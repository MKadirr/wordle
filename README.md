# Wordle helper

This programme is a side project aiming to solve Wordle.

## Wordle

Wordle is a puzzle game frome the NY times. The goal is to find an hidden 5 letters word.
To do so you are allowed to guess 6 times. When answering you will be given back informations abouts the letters you used.
For example if the hidden word is `apple` and you submit `after` you will be given the answer: `GBBYB`.

G = green: The letter used is the correct one
Y = yellow: The letter is present in the hiddent word but not at this place
B = Black: The letter is not present in the hidden word.

Example:
```
Hidden word: Apple

> After
< GBBYB

> Night
< BBBBB

> Block
< BYNNN

> Apple
< GGGGG
```

Note:

Wordle have hard mode requiring you to reuse the letter green and yellow from your previous answer. This game mode is pretty difficult even for computer since you could be stuck within a

## The solver

### Vocabulary

Hidden word: the secret word we are searching for
Used word: word used as answer at a given point
Available word: Word that are possible possible answer for the given wordle

Combinaison: this is the result of using the *Used word* X when Y is the *Hidden word*

### Maths definitions


W: set of 5 letters word from english. W in [0, 25] ^ 5
A: set of available words
H: set of valid hard word
C: the set of combination

Lets use Combi(X, Y): W * W -> C the function that for each **x** word, return the combinaison if the hidden word is **y**.

### Algorithm

The solver use a Information Theorie as heuristique too find the best word by dividing the remainings word available. 
For each 5 letters word from the english dictionary, 
We simulate it to get the combinaise of each word Y as Hidden word. 
That allow us to split the availables word into categories: let define p(c) as the ratio: Card({ X | Combi(X, Y) == c, X inc W, Y inc  }) / Card(A), whre C
With p we can create the score formula: Score E = Sum({ p(c) * log(1 / p(c)) | c in C && p(c) != 0 })
















