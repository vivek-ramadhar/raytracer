import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# Read data from sphere_plot.cc > points.txt
data = np.loadtxt('~/raytracer/points.txt')

# Extract (x,y,z) coords
x = data[:,0]
y = data[:,1]
z = data[:,2]

# Create 3d plot
fig = plt.figure(figsize=(10,8))
ax = fig.add_subplot(111, projection='3d')

# Plot points
ax.scatter(x, y, z, c=z, cmap='viridis', s=3)

# Add labels and title
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.set_title('3D Point Distribution')

# Set equal aspect ratio
ax.set_box_aspect([1, 1, 1])


# Show the plot
plt.tight_layout()
plt.show()
