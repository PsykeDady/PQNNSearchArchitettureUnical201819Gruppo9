function dist = alldists(x,y)
    n = size(x,1);
    m = size(y,1);
    sx2 = sum(x.^2,2)*ones(1,m);
    sy2 = ones(n,1)*sum(y.^2,2)';
    dist = sqrt(sx2-2*x*y'+sy2);
end
