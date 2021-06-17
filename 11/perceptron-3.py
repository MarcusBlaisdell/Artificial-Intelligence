samples = [ ([1.0, 1.0, 1.0], 1.0), \
            ([1.0, 2.0, 1.0], 1.0), \
            ([1.0, 2.0, 1.0], 0.0), \
            ([2.0, 1.0, 2.0], 1.0), \
            ([2.0, 1.0, 1.0], 1.0), \
            ([2.0, 2.0, 2.0], 1.0), \
            ([2.0, 2.0, 1.0], 0.0), \
            ([3.0, 1.0, 2.0], 1.0), \
            ([3.0, 1.0, 1.0], 1.0), \
            ([3.0, 2.0, 1.0], 0.0) ]

weight = [1.0, 1.0, 1.0, 1.0]
weightOld = [1.0, 1.0, 1.0, 1.0]
a = 0.5

def h (sample, weight):
    retval = (weight[0] * sample[0][0] + \
           weight[1] * sample[0][1] + \
           weight[2] * sample[0][2] + \
           weight[3] * sample[0][3] )
    print "sum", retval
    if retval >= 0:
        return 1.0
    else:
        return 0.0

def thePass ():
    for sample in samples:
        myUpdate = sample[1] - h(sample, weight)
        if myUpdate == 0:
            print "no update"
        else:
            print "updated"
            
        weight[0] = weight[0] + a * (myUpdate * sample[0][0])
        weight[1] = weight[1] + a * (myUpdate * sample[0][1])
        weight[2] = weight[2] + a * (myUpdate * sample[0][2])
        weight[3] = weight[3] + a * (myUpdate * sample[0][3])

        print weight

for i in range (4):
    print "\n\n"
    print "         pass: ", i + 1
    thePass ()
