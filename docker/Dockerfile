FROM ubuntu:latest
RUN apt-get update && apt-get install -y supervisor
RUN apt-get install -y --no-install-recommends rsyslog
COPY somn.conf /etc/rsyslog.d/
COPY supervisord.conf /etc/supervisor/conf.d/supervisord.conf
COPY somnd /usr/sbin/somnd

RUN rm -rf /var/lib/apt/lists/*

RUN chmod +x /usr/sbin/somnd && \
    mkdir -p /var/log/supervisor && \
    mkdir -p /var/run

CMD ["/usr/bin/supervisord", "-c", "/etc/supervisor/conf.d/supervisord.conf"]