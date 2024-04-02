FROM emscripten/emsdk:3.1.56 AS base

RUN mkdir -p scc
WORKDIR /scc

# =================================
from base as build

COPY src/ src/
COPY bench/ bench/
COPY scc_tests/ scc_tests/
COPY CMakeLists.txt .

ENV BUILD_TYPE=Release
ENV BUILD_THREADS=10
COPY build.sh .
RUN ./build.sh wasm

# =================================

FROM base AS scc
COPY --from=build /scc/build/wasm/web/ /scc/

EXPOSE 6969
CMD ["python3", "-m", "http.server", "6969"]