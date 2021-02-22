
import pathing, time, tkinter, random, time, turtle, json
import numpy as np
import matplotlib.pyplot as plt

POINTS = 40
SIZE=40

POS=(9, 9)

with open("demoMazes/maze.json") as f:
    arr = np.array(json.load(f))
    

class point(object):
    def __init__(self, goal, pos, clus, ofset=(500/SIZE, 500/SIZE), size=10):
        self.goal = goal
        self.pos = (0, *pos)
        self.ofset = ofset
        self.size=size/2
        self.clus = clus
        self.id = None
        self.turt = turtle.Turtle()
        self.turt.penup()
        self.turt.shape("circle")
        self.turt.shapesize(5,5)

        self.dx = 0
        self.dy = 0

    def show(self):
        self.turt.up()
        self.turt.setpos(self.pos[1], self.pos[2])

    def move(self):
        if self.pos[1]<0:
            self.dy*=-1
            self.pos = self.pos[0], 0, self.pos[2]

        if self.pos[1]>=SIZE:
            self.dy*=-1
            self.pos = self.pos[0], SIZE, self.pos[2]
            
        if self.pos[2]<0:
            self.dx*=-1
            self.pos= self.pos[0], self.pos[1], 0
            
        if self.pos[2]>=SIZE:
            self.dx*=-1
            self.pos = self.pos[0], self.pos[1], SIZE

        try:
            if arr[int(self.pos[0]), int(self.pos[1] + self.dy), int(self.pos[2])]==0:
               self.dy*=-1
        except:
            pass

        try:
            if arr[int(self.pos[0]), int(self.pos[1]), int(self.pos[2] + self.dx)]==0:
               self.dx*=-1
        except:
            pass
            

            
        self.pos = (0, self.pos[1]+self.dy, self.pos[2]+self.dx)
        try:
            currentNode = self.clus.getnode((int(self.pos[0]),
                                             int(self.pos[1]),
                                             int(self.pos[2])))
        except:
            return

        try:
            newNode = self.goal.getNext(currentNode)
        except:
            return
        ofset = (-currentNode.position + newNode.position)*.001*SIZE*random.random()

        self.dx += ofset[2]
        self.dy += ofset[1]

        self.dx *= .99
        self.dy *= .99
        
        
        

plt.imshow(arr[0]);plt.show()

clus = pathing.nodeGraph.Cluster()
clus.build(arr, pathing.directions.towObstacleBlock)


goal      = pathing.nodeGraph.goalCluster(clus, True)
goal.goal = clus.getnode((0,*POS))

master = tkinter.Tk()


points = []
for n in range(POINTS):
    pos = (random.randint(0, SIZE-1), random.randint(0, SIZE-1))
    if arr[0, pos[0], pos[1]]:
        points.append(point(goal, pos, clus, size=100))

s = turtle.Screen()

def updateloop():
    while True:
        t = time.time()
        update()
        dt = time.time() - t
        if dt < .1: time.sleep(.1-dt)

def update():
    for point in points:
        point.move()
        point.show()
    s.update()

s.setworldcoordinates(0,0, SIZE,SIZE)
s.tracer(0)

canvas = turtle.getcanvas()
t = turtle.Turtle()
t.hideturtle()
t.fillcolor("blue")
for y, row in enumerate(arr[0]):
    for x, val in enumerate(row):
        try:
            clus.getnode((0, x, y))
        except:
            t.up()
            t.setpos(x,y)
            t.pd()
            t.begin_fill()
            t.setpos(x,   y+1)
            t.setpos(x+1, y+1)
            t.setpos(x+1, y)
            t.setpos(x, y)
            t.end_fill()
            
        

def rebuild(x, y):
    t = time.time()
    goal.goal = clus.getnode((0, x, y))
    print(time.time()-t)
    

but = tkinter.Button(master, command=updateloop)
but.pack()

s.onclick(rebuild)
