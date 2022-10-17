import spam
from tabulate import tabulate
result = spam.retorno_memoria()
print(result)
resultado = []
print(type(result))
for i in result:
    if i != 0:
        resultado.append(i)
print(tabulate(resultado, headers=["Process", "Name", "Memoria", "Disco", "CPU","Priority"]))