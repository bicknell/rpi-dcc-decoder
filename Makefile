
dcc_decode: dcc_decode.c
	gcc -Wall -o dcc_decode dcc_decode.c -l pigpio -l pthread

clean:
	rm dcc_decode
