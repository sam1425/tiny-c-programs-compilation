# draw a tree!
#     *
#    ***
#   *****
#  *******
# *********
#     *

# height = 6
#  6 * 2 = 12 - 1 = 11
iuser = input(int())

def tree(height):
    length = height * 2 - 1
    stars = 1
    for i in range(1, height + 1):
        print (("*" * stars).center(length))
        starts += 2
tree(iuser)