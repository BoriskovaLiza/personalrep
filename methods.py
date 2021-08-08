import numpy as np
from matplotlib import pyplot as plt

def l2(x, h):
    return (sum([x_i**2*h for x_i in x]))**0.5

def init(m, N, Cu, idx_t, idx_s):
    """
    --------------
    Initiates the problem
    du/dt + c*du/dx = 0;
    c > 0, c = 1;
    w/o BC
    --------------
    Returns (x, h, dt, u, U)
    """
    x = (np.arange(N+1))/N
    u = x.copy()
    u = np.where((u > 152/300) | (u < 148/300), 0, np.sin((u-148/300)*np.pi/4*300)**m)
    
    h = x[1]
    dt = compute_dt(Cu, h, idx_t, idx_s)
    U = np.zeros_like(u)
    return (x, h, dt, u, U)

#ENO/WENO coefficients for a constant space-step

C5 = [[1/5, 77/60, -43/60, 17/60, -1/20], 
    [-1/20, 9/20, 47/60, -13/60, 1/30],
    [1/30, -13/60, 47/60, 9/20, -1/20],
    [-1/20, 17/60, -43/60, 77/60, 1/5],
    [1/5, -21/20, 137/60, -163/60, 137/60],
    [137/60, -163/60, 137/60, -21/20, 1/5]]

d = [1/10, 3/5, 3/10]    
eps = 1e-6

def get_differences(u):
    diff = np.zeros((len(u),len(u)))
    
    diff[0] = u
    diff[1][:-1] = diff[0][1:] - diff[0][:-1]
    
    for i in range(2, len(u)//2):
        diff[i][:-i] = (diff[i-1][1:-i+1] - diff[i-1][:-i])/i
        
    return diff

def f(u, h, idx):
    """
    --------------
    Space scheme:
    --------------
    1, 2 - Upwind 1, 2
    3, 4 - Central difference 2, 4
    5 - ENO-5
    6 - WENO-5
    --------------
    Returns fluxes
    """
    if idx == '1': #upwind 1
        flux = (u[1:] - u[:-1])/h
        flux = np.insert(flux, 0, 0.)
        return -flux
        
    if idx == '2': #upwind 2
        flux = (u[:-2] - 4*u[1:-1] + 3*u[2:])/2/h
        flux = np.insert(flux, 0, 0.)
        flux = np.insert(flux, 0, 0.)
        return -flux
        
    if idx == '3': #central difference 2
        flux = (u[2:] - u[:-2])/2/h
        flux = np.insert(np.append(flux, [0.]), 0, 0.)
        return -flux
        
    if idx == '4': #central difference 4
        flux = (u[:-4] - u[4:] + 8*(u[1:-3]-u[3:-1]))/12/h
        flux = np.insert(np.append(flux, [0.]), 0, 0.)
        flux = np.insert(np.append(flux, [0.]), 0, 0.)
        return flux
        
    if idx == '5': #ENO-5
        flux = np.zeros_like(u, dtype=float)
        N = len(u)
        
        flux[0] = sum(C5[-1] * u[:5])

        for j in range(0, N-2):

            lb = np.where(j-5+1 > 0, j-5+1, 0)
            rb = np.where(j+5+1 < N, j+5+1, N)
            
            V = get_differences(u[lb:rb])
            r = 0
            
            for selection in range(5):
                
                if j-lb-r < 0:
                    break
                if j-lb-r+1 > len(V[0]) - 1 - selection:
                    r += 5 - selection
                    break

                if np.abs(V[selection][j-lb-r]) <= np.abs(V[selection][j-lb-r+1]):
                    r += 1
                #left-oriented scheme imposed with '<='
            
            flux[j+1] = sum(C5[r]*u[j+1-r:5+j+1-r])
        return -np.insert((flux[1:]-flux[:-1])/h, 0, 0.)
        
    if idx == '6': #WENO-5
        flux = np.zeros_like(u, dtype=float)

        flux[2:-2] = ((1/(eps + 13/12*(u[:-4] -2*u[1:-3] +u[2:-2])**2 + \
                   1/4*(u[:-4] -4*u[1:-3] +3*u[2:-2])**2)**2/10)*\
                      (u[:-4]/3 -7*u[1:-3]/6 +11*u[2:-2]/6) +\
                    (3/5/(eps + 13/12*(u[1:-3] -2*u[2:-2] +u[3:-1])**2 + \
                   1/4*(u[1:-3] -u[3:-1])**2)**2)*\
                      (-u[1:-3]/6 +5*u[2:-2]/6 +u[3:-1]/3) +\
                    (3/10/(eps + 13/12*(u[2:-2] -2*u[3:-1] +u[4:])**2 + \
                   1/4*(3*u[2:-2] -4*u[3:-1] +u[4:])**2)**2)*\
                      
                      (u[2:-2]/3 +5*u[3:-1]/6 -u[4:]/6))/\
                    ((1/(eps + 13/12*(u[:-4] -2*u[1:-3] +u[2:-2])**2 + \
                   1/4*(u[:-4] -4*u[1:-3] +3*u[2:-2])**2)**2/10)+\
                     (3/5/(eps + 13/12*(u[1:-3] -2*u[2:-2] +u[3:-1])**2 + \
                   1/4*(u[1:-3] -u[3:-1])**2)**2)+\
                     (3/10/(eps + 13/12*(u[2:-2] -2*u[3:-1] +u[4:])**2 + \
                   1/4*(3*u[2:-2] -4*u[3:-1] +u[4:])**2)**2))
            
        return -np.insert((flux[1:]-flux[:-1])/h, 0, 0.)

def step(u, h, dt, idx_t, idx_s):
    """
    --------------
    Time scheme:
    --------------
    1 - Direct Euler
    2 - Modified Euler
    3 - Heun
    4 - RK-7
    --------------
    Returns u_(n+1)
    """
    if idx_t == '1':
        return u + f(u, h, idx_s)*dt #first time order #direct Euler method
        
    if idx_t == '2':
        u_star = u + dt*f(u, h, idx_s)/2 #modified Euler method
        return u + f(u_star, h, idx_s)*dt
        
    if idx_t == '3':
        u_star = u + dt*f(u, h, idx_s)/3 #Heun
        u_double_star = u + f(u_star, h, idx_s)*dt*2/3
        return u/4 + 3*u_star/4 + 3/4*dt*f(u_double_star, h, idx_s)
        
    if idx_t == '4': #RK-7 p.208 Butcher
        f_1 = f(u, h, idx_s)
        u_2 = u + dt*f_1/3
        f_2 = f(u_2, h, idx_s)
        u_3 = u + 2/3*dt*f_2
        f_3 = f(u_3, h, idx_s)
        u_4 = u + 1/12*dt*f_1 + 1/3*dt*f_2 - 1/12*dt*f_3
        f_4 = f(u_4, h, idx_s)
        u_5 = u + 25/48*dt*f_1 - 55/24*dt*f_2 + 35/48*dt*f_3 + 15/8*dt*f_4
        f_5 = f(u_5, h, idx_s)
        u_6 = u + 3/20*dt*f_1 - 11/24*dt*f_2 - 1/8*dt*f_3 + 1/2*dt*f_4 + 1/10*dt*f_5
        f_6 = f(u_6, h, idx_s)
        u_7 = u - 261/260*dt*f_1 + 33/13*dt*f_2 + 43/156*dt*f_3 - 118/39*dt*f_4 + \
            32/195*dt*f_5 + 80/39*dt*f_6
        f_7 = f(u_7, h, idx_s)
        return u + (13*f_1 + 55*f_3 + 55*f_4 + 32*f_5 + 32*f_6 + 13*f_7)*dt/200
        
        
def compute_dt(Cu, h, idx_t, idx_s):
    """
    --------------
    Computes optimal timestep according to the LST (linear stability theory):
    --------------
    Returns dt (default Cu*h)
    """
    conv_pow = [[1, 1, 2, 2],
                [1,4/3,4/3,4/3],
                [1, 1, 1, 1]]
                
    conv_Cu = [[1, 0.5, 1, 1],
            [1, 1, 1, 1],
            [1, 1, 1, 1]]
    
    t = int(idx_t)
    s = int(idx_s)

    if t < 4 and s < 5 and Cu == 1:
        return conv_Cu[t-1][s-1]*h**conv_pow[t-1][s-1]
    
    return Cu*h
