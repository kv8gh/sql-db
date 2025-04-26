FROM gcc:13

WORKDIR /app

# Install build tools
RUN apt-get update && \
    apt-get install -y cmake make

# Copy all code
COPY . .

# Build it
RUN mkdir build && cd build && \
    cmake .. && \
    make

# Set default run command
CMD ["./build/minidb"]
