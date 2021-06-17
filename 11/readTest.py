samples = [ ([1.0, 1.0, 1.0, 1.0], 1.0), \
            ([1.0, 1.0, 2.0, 2.0], 1.0), \
            ([1.0, 1.0, 2.0, 1.0], 0.0), \
            ([1.0, 2.0, 1.0, 2.0], 1.0), \
            ([1.0, 2.0, 1.0, 1.0], 1.0), \
            ([1.0, 2.0, 2.0, 2.0], 1.0), \
            ([1.0, 2.0, 2.0, 1.0], 0.0), \
            ([1.0, 3.0, 1.0, 2.0], 1.0), \
            ([1.0, 3.0, 1.0, 1.0], 1.0), \
            ([1.0, 3.0, 2.0, 1.0], 0.0) ]

weight = [1.0, 1.0, 1.0, 1.0]
a = 0.5
w = 1

def h (sample, weight):
    retval = (weight[0] * sample[0][0] + \
           weight[1] * sample[0][1] + \
           weight[2] * sample[0][2] + \
           weight[3] * sample[0][3] )
    #print "sum", retval
    if retval >= 0:
        return (1.0, retval)
    else:
        return (0.0, retval)

def thePass (w):
    for sample in samples:
        oldweight = weight[:]
        
        retVal = h(sample, weight)
        myH = retVal[0]
        myUpdate = sample[1] - myH

        weight[0] = weight[0] + a * (myUpdate * sample[0][0])
        weight[1] = weight[1] + a * (myUpdate * sample[0][1])
        weight[2] = weight[2] + a * (myUpdate * sample[0][2])
        weight[3] = weight[3] + a * (myUpdate * sample[0][3])
        
        print "weight", w, ": ", oldweight
        print "h", w, "(", sample[0], ") = ", oldweight, " * ", sample[0], " = ", retVal[1], " = ", retVal[0]
        print "weight", w + 1, " = ", oldweight,
        print " + ", a, " * (", sample[1], " - ", myH, ") * ", \
              sample[0], " = ", weight
        

        if myUpdate == 0:
            print "no update\n"
        else:
            print "updated\n"

        w += 1


for i in range (3):
    print "\n\n"
    print "         pass: ", i + 1
    thePass (w)
