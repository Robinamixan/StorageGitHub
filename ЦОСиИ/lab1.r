#plot(time, conc)

x <- seq(0, 1, by=0.1)
y <- character(length(x))
for (k in 1:length(x)) {
  y[k] <- sin(x[k]) + cos(x[k])
}
plot(x, y, type="o")

N=8;#%Число точек
T=1;#%Интервал времени,c

dt=T/(N);
Nyq=N/(2*T)#Частота Найквиста, Гц
df=1/T;#Шаг частоты
f=1;#Частота сигнала, Гц
w=2*pi*f;

#w=2*pi;

t=seq(0,T,length=N);#Задание вектора времени
#s=2*sin(w*t/T);
s=sin(w*t/T) + cos(w*t/T);#Вычисление дискретной функции

ks  <- 0:(length(s)-1)
i   <- complex(real = 0, imaginary = 1)

for(k in 0:(N-1)) {       # compute each time point x_n based on freqs X.k
  F[k+1] <- sum(s * exp((-1)*i*2*pi*ks*k/N))/N
}

#F=fft(s)/N;#Вычисление преобразования Фурье
#subplot(211);
plot(t,s,type="b",col="blue");#Отрисовка исходной функции

#subplot(212);
plot(1:N,abs(F[1:N]),type="b",col="red")


