FROM debian:stretch
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends apt-utils
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends build-essential
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends libhidapi-dev
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends qt5-default
# The following are installed to avoid libGL error messages:
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends mesa-utils libgl1-mesa-glx libgl1-mesa-dri
