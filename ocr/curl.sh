LOG_LEVEL=$1
IMG_DIR=$2
IMG_NAME_PATTERN=$3

if [ "${LOG_LEVEL}" = "" ]; then
    LOG_LEVEL="info"
fi

if [ "${IMG_DIR}" = "" ]; then
    IMG_DIR="images"
fi

if [ "${IMG_NAME_PATTERN}" = "" ]; then
    IMG_NAME_PATTERN="Image-%d.png"
fi

URL="https://discordbotdockered.onrender.com/daily-wordle"

DEBUG() {
    if [ "${LOG_LEVEL}" = "debug" ]; then
        echo -e "\033[32m[DEBUG] $1 \033[0m"
    fi
}

INFO() {
    if [ "${LOG_LEVEL}" = "info" ] || [ "${LOG_LEVEL}" = "debug" ]; then
        echo -e "\033[34m[INFO] $1 \033[0m"
    fi
}

WARN() {
    if [ "${LOG_LEVEL}" != "fatal" ] && [ "${LOG_LEVEL}" != "error" ]; then
        echo -e "\033[33m[WARN] $1 \033[0m"
    fi
}

ERROR() {
    if [ "${LOG_LEVEL}" != "fatal" ]; then
        echo -e "\033[1;31m[ERROR] $1 \033[0m"
    fi
}

FATAL() {
    if [ "${LOG_LEVEL}" != "fatal" ]; then
        echo -e "\033[31m[FATAL] $1 \033[0m"
    fi
}

FILE() {
    echo $(printf $1 $2)
}

DOWNLOAD() {
    filename=$(printf "${IMG_NAME_PATTERN}" "$2")
    curl -s "$1" > "${IMG_DIR}/${filename}"
    DEBUG "Reponse code: $(echo $?)"
    INFO "Image save at ${IMG_DIR}/${filename}"
}

INFO "Delete previously fetch images in ${IMG_DIR}"
rm ${IMG_DIR}/* 2> /dev/null

INFO "Create ${IMG_DIR} folder if it doesn't exists"
if [ ! -d "${IMG_DIR}" ]; then
    mkdir "${IMG_DIR}"
fi

DEBUG "Log level: ${LOG_LEVEL}"
DEBUG "Image directory: ${IMG_DIR}"
DEBUG "Image name pattern: ${IMG_NAME_PATTERN}"

INFO "Start curl"
RESPONSE=$(curl -X GET -s ${URL})
INFO "Curl end"

DEBUG "Response -> ${RESPONSE}"

NUMBER=$(echo ${RESPONSE} | sed 's/.*"nb":\([0-9]\+\).*/\1/')
URLS=$(echo ${RESPONSE} | sed 's/.*\[\(.*\)\].*/\1/' | tr "," " ")
URLS=${URLS//\"}

INFO "Number of urls: ${NUMBER}"

i=0
for LINK in ${URLS}; do
    INFO "Fetch image: ${LINK}"
    DOWNLOAD "${LINK}" "$i"
    i=$((i + 1)) 
done

DEBUG "Exit with status code $i (The number of image downloaded)"
exit "$i"