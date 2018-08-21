struct segtree
{
    int l;
    int r;
    segtree* left = NULL;
    segtree* right = NULL;
    int color = 0xffffff;
    segtree(int l = 0, int r = 1) : l(l), r(r)
    {
        if(r - l > 1)
        {
            int m = (l + r) / 2;
            left = new segtree(l, m);
            right = new segtree(m, r);
        }
    }
    void fill(int l, int r, int c)
    {
        if(l >= this->r || r <= this->l)
            return;
        if(l <= this->l && r >= this->r)
            color = c;
        else
        {
            if(color >= 0)
            {
                left->color = color;
                right->color = color;
                color = -1;
            }
            left->fill(l, r, c);
            right->fill(l, r, c);
        }
    }
    void retrieve(int* arr)
    {
        if(color >= 0)
        {
            for(int i = l; i < r; i++)
                arr[i] = color;
        }
        else
        {
            left->retrieve(arr);
            right->retrieve(arr);
        }
    }
};
