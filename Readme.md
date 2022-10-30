# FAKEFN for c++


- ASSERT(Printable)
  - Data<Printable> -> Data
- PIPE(Lfn)
	- any -> any
- IOTA(init, end, step)
	- void -> vector<int>
----------------------------------------
- JOIN(separator)
	- container -> str
	- tuple -> str
	- pair -> str
- FMT
	- CONTAINER | TUPLE | PAIR -> str
- STR
	- data | STR() -> str
	- data | STR(fmt) -> str
----------------------------------------
- NETVET
	- container -> vector empty 
- TOVEC
	- container -> vector
----------------------------------------
- SLICE(init, end)
	- container -> vector
- TAKE(qtd)
	- container -> vector
- DROP(qtd)
	- container -> vector
----------------------------------------
- FILTER(fn)
	- container -> vector
- MAP(fn)
	- container -> vector
- KEYS
	- container<pair> -> vector
- VALUES
	- container<pair> -> vector
----------------------------------------
- SPLIT(delimiter)
	- string -> vector<string>
- STR2<TYPE>
	- string -> type
- TUPLEFY<TYPES...>
	- string -> tuple
----------------------------------------
- REVERSE
	- container -> vector
- SORT
	- container -> vector
- SORTBY
	- container -> vector
- SHUFFLE
	- container -> vector
----------------------------------------
- FOLD
	- container -> value
- SUM
	- container -> value
----------------------------------------
- FIND
	- &container -> iterator
- FINDIF
	- &container -> iterator
- FOREACH
	- &container -> iterator
- INDEXOF
	- container -> value
- ZIP
	- container -> container<pair>
- PRINT
	- data -> data
- STREAM
	- void -> stream
- COLLECT
	- stream -> string



vector | STR(",", "()")
5.234  | STR("%02d") 
vector<pair> | STR() 