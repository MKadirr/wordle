import requests as req
from verbose_print import print

URL = "https://discordbotdockered.onrender.com"
IMG_DIR = 'images'

def get_urls():
    response = req.get(f'{URL}/daily-wordle')
    if response.ok:
        return response.json()
    
    print("Error while fecthing")
    return { "nb": 0, "urls": [] }

def get_img(link, image_name):
    response = req.get(link, stream=True)
    if response.ok:
        with open(f'{IMG_DIR}/{image_name}', 'wb') as f:
            for chunk in response.iter_content():
                f.write(chunk)
        return True
    
    print(f"Error while fetching {response.status_code}")
    return False

def fetch(imageNameFormat : str = "Image-{0}.png", verbose = False) -> list[str]:
    urlsDict = get_urls()
    nbUrl = urlsDict["nb"]
    urls = urlsDict["urls"]

    names = []
    print(f'Number of urls: {nbUrl}')
    for i in range(nbUrl):
        image_name = imageNameFormat.format(i)
        if get_img(urls[i], image_name):
            print(f'Save at {image_name}')
            names.append((image_name, i))
    
    return names


if __name__ == "__main__":
    urlsDict = get_urls()
    nbUrl = urlsDict["nb"]
    urls = urlsDict["urls"]

    print(f'Number of urls: {nbUrl}')
    for i in range(nbUrl):
        print(f'Url[{i}]: {urls[i]}')
        image_name = f'Image-{i}.png'
        print(f'Save at {image_name}: {get_img(urls[i], image_name)}')
