import string
import logging
import subprocess
import sys
import textwrap

from luaparser import ast
from luaparser.astnodes import Assign, LocalAssign, Index, Function, Type, Call, String, Name, IndexNotation, Number, Method, Invoke

# TODO: broken parsing when declaring local variables with no value:
# ```
# local j
# ```

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger()

def add_decls(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = {}
    for n in tree_visitor.nodes:
        if isinstance(n, LocalAssign):
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                key = id(t.scope())
                seen.setdefault(key, [])
                if t.id not in [a.id for _, a in seen[key]]:
                    seen[key].append((t.scope(), t))

        elif isinstance(n, Assign):
            # global variables, always go to the root
            key = id(tree.body)
            for t in n.targets:
                if isinstance(t, Index):
                    continue
                seen.setdefault(key, [])
                if t.id not in [a.id for _, a in seen[key]]:
                    seen[key].append((tree.body, t))

    for names in seen.values():
        for block, name in names:
            block.add_declaration(name)

def rename_stdlib_calls(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if not isinstance(n, Call):
            continue
        if n.func.id in ['sin', 'cos']:
            n.func.id = f'fix32::{n.func.id}'

def transform_methods(tree):
    """
    Rewrite methods (`function tab:method() ... end`) into an assignment of lambda into a table
    ```
    tab["method"] = lambda(...)
    ```

    And method calls (`tab:method()`) into table calls with a self-argument (`tab['method'](tab)`)

    Further passes will optimize this access into a fast-field access.
    """
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    for n in tree_visitor.nodes:
        if isinstance(n, Method):
            replacement = n.replace_with_assign()
            idx = n.parent.body.index(n)
            n.parent.body[idx] = replacement
        if isinstance(n, Invoke):
            replacement = n.replace_with_idx_call()
            n.parent.replace_child(n, replacement)


def add_signatures(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    seen = []
    for n in tree_visitor.nodes:
        if not isinstance(n, Function):
            continue
        # magical 3 indents = root; need a better way to figure out if a function
        # lives inside another function
        if n.parent.parent.parent is not None:
            continue
        tree.body.add_signatures(n)

def find_static_table_accesses(tree):
    tree_visitor = ast.WalkVisitor()
    tree_visitor.visit(tree)

    values = set()
    for n in tree_visitor.nodes:
        if not isinstance(n, Index):
            continue
        if isinstance(n.idx, String):
            values.add(n.idx.s)
            n.optimized_access = True
        elif isinstance(n.idx, Name) and n.notation == IndexNotation.DOT:
            values.add(n.id)
            n.optimized_access = True
        else:
            logger.debug("Not optimizing non-string/constant value", getattr(n.idx, 'id', getattr(n.idx, 'n', n.id)))
    return sorted(values)

def transform(src, pretty=True, dump_ast=False, testing=False):
    tree = ast.parse(src)
    rename_stdlib_calls(tree)
    transform_methods(tree)
    add_signatures(tree)
    add_decls(tree)
    static_table_fields = find_static_table_accesses(tree)

    if dump_ast:
        print(ast.to_pretty_str(tree))

    _field_to_const = {x: f'FIELD_{x.upper()}' for x in static_table_fields}
    ff_len = len(static_table_fields)
    var_init = '\n'.join([f'fields["{x}"]\t= &fast_fields[{_field_to_const[x]}];' for x in static_table_fields])
    field_to_idx = '\n'.join(f'const uint16_t {_field_to_const[x]} = {i};' for i, x in enumerate(static_table_fields))
    idx_to_name = f'const TValue* idx_to_name[{ff_len}] = {{' + ', '.join(f'new TValue("{var}")' for var in static_table_fields) + '};'


    gen = string.Template('''#include "header.h"
$field_to_idx
$idx_to_name

class SpecialTable : public Table {

    public:
       TValue fast_fields[$ff_len];

        SpecialTable(std::initializer_list<std::pair<const TValue, TValue*>> values) : SpecialTable() {
            prepopulate(values);
        }

        SpecialTable() {
            for(uint16_t i=0; i<$ff_len; i++)
                fast_fields[i] = TValue::OPT_VAL();

            $var_init
        }

        // why o why does this not work when defined in Table
        inline TValue* operator[](TValue const& key) {
            if(fields.count(key)) {
              if(!fields[key]->is_opt) {
                // TT_OPT here means "optimized" -- unset
                return fields[key];
              }
              fields[key]->is_opt = false; // make NULL
              fields[key] = nullptr;
              return fields[key];
            }

            if(metatable!=NULL && metatable->fields.count("__index")) {
                auto st = std::get<SpecialTable*>(metatable->fields["__index"]->data);
                return (*st)[key];
            }

            fields[key] = new TValue();
            return fields[key];
        }

        void set(uint16_t idx, TValue val) {
            fast_fields[idx] = val;
        }

        void sub(uint16_t idx, TValue val) {
            if(fast_fields[idx].is_opt) {
                fast_fields[idx] = get(idx);
            }
            fast_fields[idx] -= val;
        }
        void inc(uint16_t idx, TValue val) {
            if(fast_fields[idx].is_opt) {
                fast_fields[idx] = get(idx);
            }
            fast_fields[idx] += val;
        }
        TValue get(uint16_t idx) {
            TValue ret = fast_fields[idx];
            if(ret.is_opt) {
                if(metatable!=NULL && metatable->fields.count("__index")) {
                    auto st = std::get<SpecialTable*>(metatable->fields["__index"]->data);
                    return st->get(idx);
                }
                auto t = TValue();
                set(idx, t);
                return t;
            }
            return ret;
        }


};
#include "impl.cpp"
    ''').substitute(field_to_idx=field_to_idx, var_init=var_init, idx_to_name=idx_to_name, ff_len=ff_len)

    ret = gen
    ret += 'namespace Game {\n'
    ret += tree.body.dump()
    ret += '\n}\n'
    if testing:
        ret += textwrap.dedent('''
        int main() {
            Game::main();
            return 0;
        }''')

    if pretty:
        ret = prettify(ret)
    return ret

def prettify(src):
    p = subprocess.Popen(['clang-format'], stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    out, err = p.communicate(input=src.encode(), timeout=1)
    return out.decode()


if __name__ == '__main__':
    with open(sys.argv[1]) as fd:
        src = fd.read()

    t = transform(src, pretty=True, dump_ast=False)
    print(t)
