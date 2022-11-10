FROM drogonframework/drogon

RUN mkdir /app

COPY . /app

RUN cd /app

WORKDIR /app

RUN mkdir build && \
    cd build && \
    cmake .. && \
    make 
WORKDIR /app/build
ENTRYPOINT [ "./my_project" ]
