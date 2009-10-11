
# test for list support

print "LIST TEST"

m = list("!a!b!c!")

print "got here!"

print m

print m.count("!")
print m.index("a")

print tuple(m[2:4])

for x in m:
    print x

d = {1: 1, 2:2 }

print d[1]

e = {"1": "1", "2": "2"}

print e["1"]

if 1 in (1,2):
    print "yes"

print "a" == "b"

print "a" == "a"


