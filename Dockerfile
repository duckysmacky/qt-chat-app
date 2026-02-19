FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y \
			build-essential \
			cmake \
			qt6-base-dev \
			libpq-dev \
			libqt6sql6-psql \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build/ -DCMAKE_BUILD_TYPE=Release -DBUILD_CLIENT=OFF
RUN cmake --build build/ --target server_app

FROM ubuntu:24.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update \
 && apt-get install -y \
			libqt6core6 \
			libqt6network6 \
			libqt6sql6-psql \
 && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /src/build/server/server_app /app/server_app

EXPOSE 8080
CMD ["/app/server_app"]
