FROM ubuntu:latest
RUN apt-get update && apt-get install -y supervisor
RUN apt-get install -y --no-install-recommends rsyslog
RUN apt-get install -y g++
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y less

COPY conf/somn.conf /etc/rsyslog.d/
COPY conf/supervisord.conf /etc/supervisor/conf.d/supervisord.conf

# COPY src /usr/sbin/src
# RUN g++ /usr/sbin/src/somnd.cc -o /usr/sbin/somnd -lcurl
# RUN rm -rf /usr/sbin/src

COPY build/somnd /usr/sbin/somnd

RUN rm -rf /var/lib/apt/lists/*

RUN chmod +x /usr/sbin/somnd && \
    mkdir -p /var/log/supervisor && \
    mkdir -p /var/run


CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]