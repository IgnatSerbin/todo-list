FROM ubuntu:22.04 AS builder

# Установка зависимостей
RUN apt-get update && \
    apt-get install -y build-essential libncurses5-dev wget

# Копируем исходники
COPY app/todo.c /root/todo.c

# Компилируем программу
WORKDIR /root
RUN gcc todo.c -o todo -lncurses

# Переходим к созданию .deb
FROM ubuntu:22.04 AS packager

# Копируем шаблон .deb
COPY deb /deb
COPY --from=builder /root/todo /deb/usr/local/bin/todo

# Устанавливаем необходимые пакеты
RUN apt-get update && \
    apt-get install -y dpkg-dev

# Создаем .deb
RUN chmod 755 /deb/DEBIAN/control && \
    dpkg-deb --build /deb /todo.deb

# Копируем результат
CMD ["sh", "-c", "cp /todo.deb /output/"]