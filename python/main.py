from ocr import isolateWordleSquares, DataType
import ocr
from fetch import fetch
import verbose_print as verbose_print
import fetch as fth
from sys import argv
import json
import os

HELP = """
Usage: python {0} [options]

Options:
    --help                                   Print this tutorial
    --verbose                                Activate the verbose mode
    -d {{DIR}} | --img-dir {{DIR}}               Set the images directory to {{DIR}} (Default is 'images')
    -n {{NAME}} | --img-name-format {{NAME}}     Give a name format for the image. It must be like this: '.*{{0}}.*\\.png', {{0}} will be replace by the index of the image.
    -o {{NAME}} | --output-format {{NAME}}       Like --img-name-format for output images.
    -t {{TYPE}} | --data-type {{TYPE}}           Choose the result datatype between MATRIX or LIST (MATRIX by default)
    -i | --with-intermediate                 Also save itermediate preproccessed images

"""

class Options:
    def __init__(self):
        self.misformed = False
        self.execute = True
        self.verbose = False
        self.imgDir = "images"
        self.nameFormat = "Image-{0}.png"
        self.outputFormat = "output-{0}.png"
        self.dataType = "MATRIX"
        self.withItermediate = False

IMG_DIR_REGEX = r'--img-dir=(.*)'

def parse_options() -> Options:
    options = Options()
    
    i = 1
    while i < len(argv):
        arg = argv[i]

        match arg:
            case "--help":
                print(HELP.format(argv[0]))
                options.execute = False
        
            case "--verbose":
                options.verbose = True
        
            case "-d" | "--img-dir":
                if i + 1 >= len(argv):
                    options.misformed = True
                    return options
                
                i += 1
                options.imgDir = argv[i]
            
            case "-n" | "--img-name-format":
                if i + 1 >= len(argv):
                    options.misformed = True
                    return options
                
                i += 1
                options.nameFormat = argv[i]
            
            case "-o" | "--output-format":
                if i + 1 >= len(argv):
                    options.misformed = True
                    return options
                
                i += 1
                options.outputFormat = argv[i]
            
            case "-t" | "--data-type":
                if i + 1 >= len(argv):
                    options.misformed = True
                    return options
                
                i += 1
                options.dataType = argv[i]

            case "-i" | "--with-intermediate":
                options.withItermediate = True
            
            case _:
                options.misformed = True
        
        i += 1
    
    return options


if __name__ == "__main__":
    options = parse_options()

    if options.misformed:
        print("-- Misformed options --")
        print(HELP.format(argv[0]))
        exit(0)

    if not options.execute:
        exit(1)
    
    fth.IMG_DIR = options.imgDir
    ocr.IMG_DIR = options.imgDir

    if not os.path.exists(options.imgDir):
        os.makedirs(options.imgDir)

    dataType = DataType[options.dataType]
    
    verbose_print.VERBOSE = options.verbose
    filenames = fetch(options.nameFormat)
    for file, i in filenames:
        corpus = isolateWordleSquares(file, options.outputFormat.format(i), dataType, options.withItermediate)
    
    print(json.dumps(corpus))