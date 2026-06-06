import pyautogui as pag
import time
import numpy as np
import mss
import cv2
import time
import keyboard
import sys


mon = {'left': 44, 'top': 277, 'width': 900, 'height': 900}
rows, cols = (30, 30)
grid = [[0 for i in range(cols)] for j in range(rows)]
totguesses = 0 
tiles = {
    (255,255,255):  10, #white
    (192,192,192):  0,  #basegrey
    (255,0,0):      1,  #blue 
    (0,128,0):      2,  #green 
    (160,184,160):  2,  #green 
    (37,141,37):    2,  #green 
    (0,0,255):      3,  #red 
    (128,0,0):      4,  #blue 
    (163,99,99):    4,  #blue 
    (134,134,134):  9,  #black 
    (0,0,0):        9,  #black 
    (67,67,67):     9,  #black 
    (134,134,211):  9,  #black
    (128,128,128):  11, #edgegrey 
    (141,141,141):  11, #edgegrey
    (0,0,128):      5,  #darkred 
    (111,111,168):  5,  #darkred 
    (32,32,139):    5,   #darkred
    (128,128,0):    6,   #teal
    (139,139,32):   6    #teal
    }



def getTileFromPixel(center, top, botright, topleft):
    center_ = tiles.get(center)
    top_   = tiles.get(top)
    botleft_ = tiles.get(botright)
    topleft_ = tiles.get(topleft)
    if botleft_ == 5:
        center_ = botleft_
    if center_ == 0 and top_ == 10:
        center_ = top_
    if topleft_ == 3:
        center_ = 9
    if botleft_ == 9:
        printGrid()
        sys.exit("oh no, you lost your legs :(\nTOTAL GUESSES: " + str(totguesses))
    return center_




def processImg(img):
    relevantTiles = {}
    for i in range(30):
        for j in range(30):
            iC = i*30
            jC = j*30
            centerPixel = img[iC+15, jC+17] 
            topPixel    = img[iC+15, jC+2]
            botrightPixel    = img[iC+17, jC+18]
            topleftPixel = img[iC+13, jC+14]

            tile = getTileFromPixel(tuple(centerPixel), tuple(topPixel), tuple(botrightPixel), tuple(topleftPixel))
            grid[i][j] = tile  # type: ignore
            if tile == 1 or tile == 2 or tile == 3 or tile == 4 or tile == 5 or tile == 6:
                relevantTiles[(i,j)] = tile
    return relevantTiles
        








def nextMove(tiles):
    toBeFlagged = set()
    toBeClicked = set()
    probabilities = {}
    guesses = 0
    for key in tiles:
        flag = 0
        tile = tiles[key]
        row, col = key
        unkown = []
        for i in range(-1,2):
            for j in range(-1,2):
                if row+i < 0 or row+i > 29 or col+j < 0 or col+j > 29: continue
                if i == j == 0: continue
                if grid[row+i][col+j] == 10: unkown.append((col+j,row+i))
                if grid[row+i][col+j] == 9: flag += 1

        #   CALCULATE PROBABILITIES   

        if len(unkown) > 0 and tile > flag:
            chance = round((tile-flag)/len(unkown),2)
            for coord in unkown:
                if coord not in probabilities: probabilities[coord] = chance
                if chance > probabilities[coord]: probabilities[coord] = chance
            
        #   ADD TILES TO BE CLICKED

        if len(unkown) > 0 and tile == flag:
            toBeClicked.add((col,row))
            
        #   ADD TILES TO BE FLAGED

        if len(unkown) == tile - flag:
            for coord in unkown:
                toBeFlagged.add(coord)
                
    #   FLAG TILES

    for coord in toBeFlagged:
        pag.click(button='right', x=mon['left'] + coord[0]*30 + 15, y=mon['top'] + coord[1]*30 + 15)
        grid[coord[1]][coord[0]] = 9

    #   CLICK TILES

    for coord in toBeClicked:
        pag.click(button='middle',x=mon['left'] + coord[0]*30 + 15, y=mon['top'] + coord[1]*30 + 15)

    #   IF NO OTHER CHOICE → CHOOSE TILE WITH LOWEST CHANCE OF BEEING BOMB

    if len(toBeClicked) == 0 and len(toBeFlagged) == 0:
        pick = (15,15)
        lowestchance = 1.0
        for key in probabilities:
            thischance = probabilities[key]
            if thischance < lowestchance:
                lowestchance = thischance
                pick = key
        pag.click(x=mon['left'] + pick[0]*30 + 15, y=mon['top'] + pick[1]*30 + 15)
        guesses += 1

    return guesses, probabilities
    


def printGrid():
    for i in grid:
        for j in i:
            if j == 1:      print("1 ",end="")
            elif j == 2:    print("2 ",end="")
            elif j == 3:    print("3 ",end="")
            elif j == 4:    print("4 ",end="")
            elif j == 5:    print("5 ",end="")
            elif j == 10:   print("# ",end="")
            elif j == 0:    print("  ",end="")
            elif j == 9:    print("⚑ ",end="")
            elif j == 6:    print("6 ",end="")
        print("")





def screenshot(sct):
    screenshot = sct.grab(mon)
    img = np.array(screenshot)
    img = cv2.cvtColor(img, cv2.COLOR_BGRA2BGR)
    tiles = processImg(img)
    return nextMove(tiles)


img = np.array
probabilities = set()

pag.click(x=mon['left'] + 15*30 + 15, y=mon['top'] + 15*30 + 15)

with mss.mss() as sct:
    while True:

        if keyboard.is_pressed('e'):
            printGrid()
            print("TOTAL GUESSES: ",totguesses)
            break

        guesses, probabilities = screenshot(sct)
        totguesses += guesses
   
        if len(probabilities) == 0:
            printGrid()
            print("you made it, phew!\nTOTAL GUESSES: ",totguesses)
            break
        


        
