FROM ubuntu:latest
RUN apt-get update && apt-get install -y supervisor
RUN apt-get install -y --no-install-recommends rsyslog
RUN apt-get install -y g++
RUN apt-get install -y libcurl4-openssl-dev
RUN apt-get install -y less
RUN apt-get install -y iproute2

COPY conf/somn.conf /etc/rsyslog.d/
COPY conf/supervisord.conf /etc/supervisor/conf.d/supervisord.conf


COPY build/somnd /usr/sbin/somnd



RUN chmod +x /usr/sbin/somnd && \
    mkdir -p /var/log/supervisor && \
    mkdir -p /var/run


CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]