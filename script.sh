#COMPILA
gcc -o programa BlackScholes.c -lm

#VARIAVEIS
ITERACOES=30

if [ ! -d "Resultados" ];then
    mkdir Resultados
else
    rm -r Resultados #limpa para inserir novos resultados
    mkdir Resultados 
fi

for (( i=0; i<$ITERACOES; i++ ));
do
	echo "Executando iteracao $i..."
	./programa 100 100 0.05 0.2 1 100 < entrada.txt >> Resultados/saida.txt
	#/usr/bin/time -f "%e" ./programa < entrada.txt >> Resultados/saida.txt
done

mv tempo.txt Resultados
gcc -o programa calculos.c -lm
echo "Executando Calculos..."
./programa $ITERACOES Resultados/tempo.txt

mv Calculo.txt Resultados
echo "Execucao finalizada!"

rm programa
