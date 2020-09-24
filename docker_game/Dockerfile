FROM ubuntu
WORKDIR /root/
COPY game /root/
COPY game_start.sh /root/
COPY random_first.txt /root/
COPY random_last.txt /root/
EXPOSE 8899
ENTRYPOINT [ "./game_start.sh" ]