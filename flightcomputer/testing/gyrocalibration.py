import numpy as np
import matplotlib.pyplot as plt





def Elipse(center, nPoints, a,b,c):
    theta = 2 * np.pi * np.random.rand(nPoints) # random angle
    phi = np.arccos(1 - 2 * np.random.rand(nPoints)) # random angle
    # spherical to cartesianjklJ
    x = a * np.sin(phi) * np.cos(theta)
    y = b * np.sin(phi) * np.sin(theta)
    z = c * np.cos(phi)
    x = x + center[0]
    y = y + center[1]
    z = z + center[2]
    return x, y, z


def Elipse2d(center, axis_a , axis_b, nPoints):
    x0, y0 = center
    a1, b1 = axis_a
    a2, b2 = axis_b
    t = np.linspace(0, 2*np.pi, nPoints)
    x = x0 + a1*np.cos(t) + a2*np.sin(t)
    y = y0 + b1*np.cos(t) + b2*np.sin(t)

    return x, y





x,y,z = Elipse([0,1,2], 200, 2,2,4)

fig = plt.figure(dpi=100)
ax = fig.add_subplot(111, projection='3d')
ax.scatter(x,y,z)
ax.set_xlim(-5,5)
ax.set_ylim(-5,5)
ax.set_zlim(-5,5)
plt.show()


x = np.reshape(x,(-1,1))
y = np.reshape(y,(-1,1))
z = np.reshape(z,(-1,1))
A = np.concatenate((x,y,z),axis=1)
# bias = np.mean(A,axis=0)
# A = A - bias
# print(A.shape)
# U, s, V = np.linalg.svd(A, full_matrices=False)
# V = V.T * 2
# print(U.shape)
# print(s.shape)
# print(V.shape)
#    
# fig = plt.figure(dpi=100)
# ax = fig.add_subplot(111, projection='3d')
# ax.plot([0,V[0,0]],[0,V[1,0]],[0,V[2,0]])
# ax.plot([0,V[0,1]],[0,V[1,1]],[0,V[2,1]])
# ax.plot([0,V[0,2]],[0,V[1,2]],[0,V[2,2]])
# ax.scatter(A[:,0],A[:,1],A[:,2])
# ax.set_xlim(-5,5)
# ax.set_ylim(-5,5)
# ax.set_zlim(-5,5)
# plt.show()
#
#
# corrected = A @ V @ np.linalg.inv(np.diag(s)) @ V.T
#
# fig = plt.figure(dpi=100)
# ax = fig.add_subplot(111, projection='3d')
# ax.scatter(corrected[:,0],corrected[:,1],corrected[:,2])
#
# plt.show()






def ellipsoid_fit(X):
    x = X[:, 0]
    y = X[:, 1]
    z = X[:, 2]
    # elipsoid x^2/a^2 + y^2/b^2 + z^2/c^2 = 1
    D = np.array([x * x + y * y - 2 * z * z,
                 x * x + z * z - 2 * y * y,
                 2 * x * y,
                 2 * x * z,
                 2 * y * z,
                 2 * x,
                 2 * y,
                 2 * z,
                 1 - 0 * x])
    d2 = np.array(x * x + y * y + z * z).T # rhs for LLSQ
    u = np.linalg.solve(D.dot(D.T), D.dot(d2))
    a = np.array([u[0] + 1 * u[1] - 1])
    b = np.array([u[0] - 2 * u[1] - 1])
    c = np.array([u[1] - 2 * u[0] - 1])
    v = np.concatenate([a, b, c, u[2:]], axis=0).flatten()
    A = np.array([[v[0], v[3], v[4], v[6]],
                  [v[3], v[1], v[5], v[7]],
                  [v[4], v[5], v[2], v[8]],
                  [v[6], v[7], v[8], v[9]]])

    center = np.linalg.solve(- A[:3, :3], v[6:9])

    translation_matrix = np.eye(4)
    translation_matrix[3, :3] = center.T

    R = translation_matrix.dot(A).dot(translation_matrix.T)

    evals, evecs = np.linalg.eig(R[:3, :3] / -R[3, 3])
    evecs = evecs.T

    radii = np.sqrt(1. / np.abs(evals))
    radii *= np.sign(evals)

    return center, evecs, radii, v




center, evecs, radii, v = ellipsoid_fit(A)

print(center)
print(evecs)
print(radii)
print(v)
B = A - center
corrected = B @ evecs.T @ np.linalg.inv(np.diag(radii)) @ evecs 

fig = plt.figure(dpi=100)
ax = fig.add_subplot(111, projection='3d')
ax.scatter(corrected[:,0],corrected[:,1],corrected[:,2])
ax.scatter(A[:,0],A[:,1],A[:,2])
ax.set_xlim(-5,5)
ax.set_ylim(-5,5)
ax.set_zlim(-5,5)
ax.set_aspect('equal')
plt.show()
