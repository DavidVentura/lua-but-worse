import gdb

# enum typetag_t {NUL=0, STR=1, TAB=2, FUN=3, NUM=4, BOOL=5};

class FixPrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        return gdb.parse_and_eval(f'fix32_to_float(fix32_from_parts({self.val["i"]}, {self.val["f"]}))')

class StrPrinter:
    def __init__(self, val):
        self.val = val

    def _read_contents(self):
        return gdb.selected_inferior().read_memory(self.val['data'], self.val['len']).tobytes().decode()

    def to_string(self):
        return self._read_contents()

class TVSlicePrinter:
    def __init__(self, val):
        self.val = val

    def children(self):
        size = self.val['num']
        yield 'num', size
        for i in range(size):
            el_ptr = self.val['elems'] + i
            el = el_ptr.dereference()
            yield 'el', el

class KVSlicePrinter:
    def __init__(self, val):
        self.val = val

    def children(self):
        size = self.val['len']
        for i in range(size):
            kv_ptr = self.val['kvs'] + i
            kv = kv_ptr.dereference()
            if kv['key']['tag'] != 0:
                yield 'kv', kv

    def display_hint(self):
        return "array"

class KVPrinter:
    def __init__(self, val):
        self.val = val
        self._seen = set()

    def to_string(self):
        if str(self.val['key']) == '__index':
            return '<refusing to print __index>'
        if self.val.address in self._seen:
            return '<recursion>'
        self._seen.add(self.val.address)
        return f"{self.val['key']} = {self.val['value']}"

class TablePrinter:
    def __init__(self, val):
        self.val = val

    def children(self):
        meta = None
        meta_idx = self.val['metatable_idx']
        if meta_idx != 65535:
            #if meta_idx == idx:
            #    meta = 'self'
            #else:
            meta = gdb.parse_and_eval(f'_tables.tables[{meta_idx}]')
            if meta.address == self.val.address:
                meta = 'self'
        else:
            meta = 'nil'
        keycount = self.val['count']
        size = self.val['kvp']['len']
        yield ("keys#", f'{keycount}/{size}')
        yield ("meta", meta)
        if keycount > 0:
            yield ('kvs', self.val['kvp'])

class TValuePrinter:
    def __init__(self, val):
        self.val = val

    def to_string(self):
        tag = self.val['tag']

        if tag == 0: 
            return "nil TValue"
        elif tag == 1:
            idx = self.val['str_idx']
            _str_obj = gdb.parse_and_eval(f'_strings.strings[{idx}]')
            return str(_str_obj)
        elif tag == 2:  # table_idx
            idx = self.val['table_idx']
            result = gdb.parse_and_eval(f'_tables.tables[{idx}]')
            return str(result)
        elif tag == 3:  # fun
            return self.val['fun']
        elif tag == 4:
            num = self.val["num"]
            return str(num)
        elif tag == 5:
            return self.val['num']['i'] == 1
        else:
            return "something went terribly wrong"

def tvalue_t_lookup(val):
    if str(val.type) == "TValue_t":
        return TValuePrinter(val)
    elif str(val.type) == "KV_t":
        return KVPrinter(val)
    elif str(val.type) == "Table_t":
        return TablePrinter(val)
    elif str(val.type) == "Str_t":
        return StrPrinter(val)
    elif str(val.type) == "fix32_t":
        return FixPrinter(val)
    elif str(val.type) == "KVSlice_t":
        return KVSlicePrinter(val)
    elif str(val.type) == "TVSlice_t":
        return TVSlicePrinter(val)
    return None

def register_tvalue_s_printer():
    gdb.pretty_printers.append(tvalue_t_lookup)

register_tvalue_s_printer()

