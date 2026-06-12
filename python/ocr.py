from PIL import Image
from enum import Enum
from collections import Counter
from colorsys import rgb_to_hsv
from verbose_print import print

IMG_DIR = "images"

class Color(Enum):
    BLACK=(0, 0, 0, 255)
    GRAY=(120, 120, 120, 255)
    GREEN=(80, 140, 80, 255)
    YELLOW=(180, 160, 60, 255)
    WHITE=(255, 255, 255, 255)


def rgba_to_color_name(rgba : tuple):
    r, g, b = 0, 0, 0
    if len(rgba) == 3:
        r, g, b = rgba
    else:
        r, g, b, a = rgba

    # Normalize to 0-1
    r, g, b = r / 255, g / 255, b / 255

    h, s, v = rgb_to_hsv(r, g, b)

    # Convert hue to degrees
    h = h * 360

    # Low saturation = grayish colors
    if s < 0.15:
        if v < 0.2:
            return Color.BLACK
        else:
            return Color.GRAY

    # Hue ranges
    if 30 <= h < 90:
        return Color.YELLOW
    elif 90 <= h < 150:
        return Color.GREEN
    
    return Color.BLACK

class Square:
    def __init__(self, start : tuple[int, int], sizeX : int, sizeY : int, color : Color):
        self.start = start
        self.sizeX = sizeX
        self.sizeY = sizeY
        self.color = color
    
    def fill(self, matrix : list[list[Color]], color : Color):
        x, y = self.start

        for xi in range(x + self.sizeX):
            for yi in range(y + self.sizeY):
                matrix[yi][xi] = color
    
    def similar(self, square):
        if not isinstance(square, Square):
            return False
        return self.sizeX == square.sizeX and self.sizeY == square.sizeY

    def __eq__(self, value):
        if not isinstance(value, Square):
            return False
        return self.start[0] == value.start[0] and self.start[1] == value.start[1]
    
    def __repr__(self):
        return f'S{{{self.start}}}[{self.sizeX}, {self.sizeY}] -- {self.color}'
    
    def toNumber(self):
        match self.color:
            case Color.GRAY:
                return 0
            case Color.GREEN:
                return 1
            case Color.YELLOW:
                return 2
            case _:
                return -1

class DataType(Enum):
    MATRIX = "matrix"
    LIST = "list"

class SquareCorpus:
    def __init__(self, first : Square = None):
        self.corpus : list[list[Square | None]] = [[ None ] * 5 for _ in range(6)] # Wordle grid
        self.offset : int | None = None
        if first is None:
            self.index : int = 0
        else:
            self.index : int = 1
            self.corpus[0][0] = first
    
    def respectXOffset(self, prec : Square, elm : Square) -> bool:
        return elm.start[0] - prec.start[0] - prec.sizeX == self.offset
    
    def respectYOffset(self, prec : Square, elm : Square) -> bool:
        return elm.start[1] - prec.start[1] - prec.sizeY == self.offset

    def add(self, elm : Square) -> bool:

        xi, yi = self.index % 5, self.index // 5

        if self.index == 1:
            first = self.corpus[0][0]
            if first.start[1] != elm.start[1] or not first.similar(elm):
                return False
            
            self.offset = elm.start[0] - first.start[0] - first.sizeX
        
        if self.index >= 2:
            if xi == 0:
                prec = self.corpus[yi - 1][0]
                if prec.start[0] != elm.start[0] or not prec.similar(elm) or not self.respectYOffset(prec, elm):
                    return False
            else:
                prec = self.corpus[yi][xi - 1]
                if prec.start[1] != elm.start[1] or not prec.similar(elm) or not self.respectXOffset(prec, elm):
                    return False
        
        self.corpus[yi][xi] = elm
        self.index += 1
        return True
    
    def removeListElmFromCorpus(self, squares: list[Square]):
        for line in self.corpus:
            for square in line:
                if square is not None:
                    squares.remove(square)

    def toList(self) -> list[Square]:
        listSquares = []
        for l in self.corpus:
            listSquares.extend(l)
        
        return listSquares

    def isValidCorpus(self):
        return self.index == 30
    
    def toData(self, dataType : DataType = DataType.MATRIX):
        match dataType:
            case DataType.MATRIX:
                return [
                    [ s.toNumber() for s in l ] for l in self.corpus
                ]
            case DataType.LIST:
                return [ s.toNumber() for l in self.corpus for s in l ]


def find_gray_square(start : tuple[int, int], matrix : list[list[Color]], maxs : tuple[int, int]) -> Square | None:
    x, y = start
    sx, sy = 0, 0
    maxX, maxY = maxs
    while maxX > x + sx and matrix[y][x + sx] == Color.GRAY:
        sx += 1

    while maxY > y + sy and matrix[y + sy][x + sx - 1] == Color.GRAY:
        sy += 1
    
    if sx != sy:
        return None
    
    # print(f'Square at {start}: {sx} -- {sy}')

    for xi in range(x, x + sx):
        for yi in range(y, y + sy):
            if matrix[yi][xi] != Color.GRAY and matrix[yi][xi] != Color.BLACK:
                return None
    
    midX = x + int(sx / 2)
    midY = y + int(sy / 2)
    color = matrix[midY][midX]
    if color == Color.BLACK:
        color = Color.WHITE
    return Square(start, sx, sy, color)

def find_mono_color_square(start : tuple[int, int], matrix : list[list[Color]], color : Color, maxs : tuple[int, int]) -> Square | None:
    x, y = start
    sx, sy = 0, 0
    maxX, maxY = maxs
    while maxX > x + sx and maxY > y + sy and matrix[y + sy][x + sx] == color:
        sx += 1
        sy += 1

    # print(f'Sx: {sx} -- Sy: {sy}')
    
    for xi in range(x, x + sx):
        for yi in range(y, y + sy):
            if matrix[yi][xi] != color:
                return None
    
    return Square(start, sx, sy, color)

def find_square(start : tuple[int, int], matrix : list[list[Color]], color : Color, maxs : tuple[int, int]) -> Square | None:
    if color == Color.GRAY:
        return find_gray_square(start, matrix, maxs)
    return find_mono_color_square(start, matrix, color, maxs)

def find_squares(matrix : list[list[Color]]):
    height = len(matrix)
    width = len(matrix[0]) if height > 0 else 0
    squares = []

    print(f'Find squares of matrix {width}x{height}')

    x, y = 0, 0
    while y < height:
        x = 0
        while x < width:
            if matrix[y][x] != Color.BLACK:
                square = find_square((x, y), matrix, matrix[y][x], (width, height))
                if square is not None:
                    squares.append(square)
                    square.fill(matrix, Color.BLACK)
            x += 1
        y += 1
    
    print(f'Number of squares: {len(squares)}')
    return squares

def find_square_corpus(squares : list[Square]) -> list[SquareCorpus]:
    squares.sort(key=lambda s: (s.start[1], s.start[0]))
    corpus = []
    while len(squares) > 0:
        c = SquareCorpus(squares[0])
        for i in range(1, len(squares)):
            if c.add(squares[i]):
                # print(f'Valid corpus square {squares[i].start} in {c.corpus[0][0].start}')
                pass
        
        if c.isValidCorpus():
            corpus.append(c)
        
        c.removeListElmFromCorpus(squares)

    return corpus

def filter_squares(squares : list[Square], minSize = 4, maxSize = 25):
    squares.sort(key=lambda s: s.sizeX)
    nbSquares = len(squares)
    while nbSquares > 0 and squares[nbSquares - 1].sizeX > maxSize:
        nbSquares -= 1
        squares.pop()
    
    while nbSquares > 0 and squares[0].sizeX < minSize:
        nbSquares -= 1
        squares.pop(0)
    
    squareSizes = []
    for s in squares:
        squareSizes.append(s.sizeX)
    
    keepSize = []
    for size, count in Counter(squareSizes).items():
        if count >= 30:  # Nombre de case minimum au Wordle
            keepSize.append(size)
    
    return [ s for s in squares if s.sizeX in keepSize ]

def toMatrix(image : Image):
    print(f'Image of size: {image.width}x{image.height}')
    matrix = []
    for y in range(image.height):
        line = []
        for x in range(image.width):
            nColor = rgba_to_color_name(image.getpixel((x, y)))
            line.append(nColor)
        matrix.append(line)
    return matrix

def squares_to_matrix(width : int, height : int, squares : list[Square]):
    matrix = [[Color.BLACK for _ in range(width)] for _ in range(height)]
    # print(squares)
    for s in squares:
        startX, startY = s.start
        for x in range(startX, startX + s.sizeX):
            for y in range(startY, startY + s.sizeY):
                matrix[y][x] = s.color
    return matrix

def toImg(matrix : list[list[Color]]) -> Image:
    height = len(matrix)
    width = len(matrix[0]) if height > 0 else 0
    image = Image.new("RGB", (width, height))
    for y in range(height):
        for x in range(width):
            image.putpixel((x, y), matrix[y][x].value)
    return image

def isolateWordleSquares(imageFilename, resultFilename, dataOutputType : DataType = DataType.MATRIX, withIntermediate = False):
    image = Image.open(f"{IMG_DIR}/{imageFilename}")
    imgMatrix = toMatrix(image)

    if withIntermediate:
        preproccesImg = toImg(imgMatrix)
        preproccesImg.save(f"{IMG_DIR}/colored-{resultFilename}")
    
    height = len(imgMatrix)
    width = len(imgMatrix[0]) if height > 0 else 0
    squares = find_squares(imgMatrix)

    if withIntermediate:
        preproccesImg = toImg(squares_to_matrix(width, height, squares))
        preproccesImg.save(f"{IMG_DIR}/raw-squares-{resultFilename}")

    squares = filter_squares(squares)

    if withIntermediate:
        preproccesImg = toImg(squares_to_matrix(width, height, squares))
        preproccesImg.save(f"{IMG_DIR}/filtered-squares-{resultFilename}")

    corpus = find_square_corpus(squares)
    print(f'Number of corpus: {len(corpus)}')
    wordleSquares = []
    for c in corpus:
        wordleSquares.extend(c.toList())
    
    preproccesImg = toImg(squares_to_matrix(width, height, wordleSquares))
    preproccesImg.save(f"{IMG_DIR}/{resultFilename}")

    return [ c.toData(dataOutputType) for c in corpus ]

if __name__ == "__main__":
    for i in range(7):
        isolateWordleSquares(f"Image-{i}.png", f"output-{i}.png")

    # isolateWordleSquares("Image-4.png", "output-4.png")
