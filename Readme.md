# FAKEFN for c++

Funções
- iota(begin, end, inc = 1)
- str2<type>

- str(data, cfmt = "")
- format(format, args...)

- split (string, )
- join(container, sep="", brakets="")

- print(data, end="\n")


---------------------------------------
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
- FORMAT
	- format | vararg 
- STR
	- data | STR(fmt) -> str
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
- SORTBY
	- container -> vector
----------------------------------------
- FOLD
	- container -> value
- FOLD1
	- container -> value
----------------------------------------
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
