FROM ubuntu:22.04 AS builder
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y build-essential cmake clang-15 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=clang++-15
RUN cmake --build build --config Release -j$(nproc)
RUN ctest --test-dir build --output-on-failure

FROM ubuntu:22.04 AS runner
WORKDIR /app
COPY --from=builder /app/build/netloop_bench /app/netloop_bench
COPY --from=builder /app/build/netloop_tests /app/netloop_tests
CMD ["./netloop_bench"]
