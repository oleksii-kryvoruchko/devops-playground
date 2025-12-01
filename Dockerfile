FROM alpine:3.20 AS builder
RUN apk add --no-cache build-base=0.5-r3
WORKDIR /app
COPY src/main.c .
RUN gcc -static -O2 -o load-tester main.c

FROM alpine:3.20
RUN apk add --no-cache stress-ng=0.17.08-r0 && rm -rf /var/cache/apk/*
COPY --from=builder /app/load-tester /app/load-tester
EXPOSE 8080
CMD ["/app/load-tester"]
