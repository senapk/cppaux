str(data, cfmt = "")
pipe (data, fn)
map  (ds, fn)
slice(ds, start, end)
strto<type>(text)
tuplefy<type...>(text)
filter(ds, fn)
cstr(text)
write(text, end = "")
split(text, del = ' ')
join(ds, sep  = " ", brackets = "")
zip (ds1, ds2)
zipwith(ds1, ds2, fn)
format(text, args...)
iota(size, init, step = 1)

input(istream)

5 | RANGE() -> [0, 1, 2, 3, 4]
range(5) -> [0, 1, 2, 3, 4]

1 | RANGE(6) -> [1, 2, 3, 4, 5]
range(1, 6) -> [1, 2, 3, 4, 5]

1 | RANGE(6, 2) -> [1, 3, 5]
range(1, 6, 2) -> [1, 3, 5]

range(end)
range(start, end, step = 1)


strings
    input pra ler linha
    write pra imprimir

    + pra converter em double
    s pra converter em string obj
    
    tostr() pra gerar string
    cstr() pra gerar string c
    
    format() pra formatar string

    conversões de string
    strto<type>() pra converter string pra tipo
    + ou number pra converter pra double
    unpack() pra converter pra tuple

    join pra juntar coisas
    split para separar coisas    

criar uma biblioteca fnextra

asserteq(received, expected, label = "")
keys(ds)        <------ useless
values(ds)      <------ useless
sort(ds)        <------ useless
reverse(ds)     <------ useless
shuffle(ds)     <------ useless
unique(ds)      <------ useless
take(ds, n)     <------ useless
drop(ds, n)     <------ useless
sortby(ds, fn)  <------ useless
cat(text, text) <------ useless
fold(ds, fn)    <------ useless, use map instead
sum (ds)        <------ useless, use fold
findif          <------ useless, use filter
fold1           <------ useless, use fold

[1,2,3,4,5,6,7,8,9]

slice(0, 0)

