import json
def metrics(names, arr, bur, fin, first):
    out=[]
    for i,n in enumerate(names):
        tat=fin[i]-arr[i]; wt=tat-bur[i]; rt=first[i]-arr[i]
        out.append((n,tat,wt,rt))
    return out
def merge(g):
    m=[]
    for p,s,e in g:
        if m and m[-1][0]==p and m[-1][2]==s: m[-1]=(p,m[-1][1],e)
        else: m.append((p,s,e))
    return m
def nonpre(names,arr,bur,key):
    n=len(names); t=0; done=[0]*n; fin=[0]*n; first=[0]*n; g=[]
    while sum(done)<n:
        rdy=[i for i in range(n) if not done[i] and arr[i]<=t]
        if not rdy: t=min(arr[i] for i in range(n) if not done[i]); continue
        i=min(rdy,key=lambda x:(key(x),arr[x]))
        first[i]=t; g.append((names[i],t,t+bur[i])); t+=bur[i]; fin[i]=t; done[i]=1
    return g,fin,first
def srtf(names,arr,bur):
    n=len(names); left=bur[:]; t=0; fin=[0]*n; first=[-1]*n; g=[]
    while any(left):
        rdy=[i for i in range(n) if left[i]>0 and arr[i]<=t]
        if not rdy: t+=1; continue
        i=min(rdy,key=lambda x:(left[x],arr[x]))
        if first[i]<0: first[i]=t
        g.append((names[i],t,t+1)); left[i]-=1; t+=1
        if left[i]==0: fin[i]=t
    return merge(g),fin,first
def rr(names,arr,bur,q):
    n=len(names); left=bur[:]; t=0; fin=[0]*n; first=[-1]*n; g=[]; Q=[]; added=[0]*n; sw=0; last=-1
    Q.append(0); added[0]=1; done=0
    while done<n:
        if not Q:
            nx=min(i for i in range(n) if not added[i]); t=max(t,arr[nx]); Q.append(nx); added[nx]=1
        p=Q.pop(0)
        if first[p]<0: first[p]=t
        r=min(q,left[p]); g.append((names[p],t,t+r)); t+=r; left[p]-=r
        if last!=p: sw+=1
        last=p
        for i in range(n):
            if not added[i] and arr[i]<=t: Q.append(i); added[i]=1
        if left[p]>0: Q.append(p)
        else: fin[p]=t; done+=1
    return merge(g),fin,first,sw-1
def mlfq(names,arr,bur,qs):
    n=len(names); left=bur[:]; lvl=[0]*n; t=0; fin=[0]*n; first=[-1]*n; g=[]
    queues=[[] for _ in qs]; added=[0]*n; done=0
    def admit():
        for i in range(n):
            if not added[i] and arr[i]<=t: queues[0].append(i); added[i]=1
    admit()
    while done<n:
        admit()
        k=next((j for j,qq in enumerate(queues) if qq),None)
        if k is None:
            t=min(arr[i] for i in range(n) if not added[i]); continue
        p=queues[k].pop(0)
        if first[p]<0: first[p]=t
        q=qs[k] if k<len(qs)-1 else 10**9
        # run slice but allow preemption by new arrival into higher queue (check at arrival times)
        run=min(q,left[p]); end=t+run
        nxt=[arr[i] for i in range(n) if not added[i] and arr[i]<end and k>0]
        if nxt:
            end=min(nxt); run=end-t
        g.append((names[p],t,end)); t=end; left[p]-=run
        admit()
        if left[p]==0: fin[p]=t; done+=1
        elif run>=q and k<len(qs)-1: queues[k+1].append(p)
        else: queues[k].insert(0,p)
    return merge(g),fin,first
def show(title,g,m):
    print("##",title)
    print("Gantt:"," ".join(f"{p}({s}-{e})" for p,s,e in g))
    for r in m: print("  %s TAT=%d WT=%d RT=%d"%r)
    print("  AVG TAT=%.2f WT=%.2f RT=%.2f"%tuple(sum(r[i] for r in m)/len(m) for i in (1,2,3)))
res={}
def run_all(label,names,arr,bur,pri,qlist,mq):
    print("=========",label)
    g,f,fr=nonpre(names,arr,bur,lambda i:0); show("FCFS",g,metrics(names,arr,bur,f,fr))
    g,f,fr=nonpre(names,arr,bur,lambda i:bur[i]); show("SJF",g,metrics(names,arr,bur,f,fr))
    g,f,fr=srtf(names,arr,bur); show("SRTF",g,metrics(names,arr,bur,f,fr))
    g,f,fr=nonpre(names,arr,bur,lambda i:pri[i]); show("Priority",g,metrics(names,arr,bur,f,fr))
    for q in qlist:
        g,f,fr,sw=rr(names,arr,bur,q); m=metrics(names,arr,bur,f,fr); show(f"RR q={q} (switches {sw})",g,m)
    g,f,fr=mlfq(names,arr,bur,mq); show(f"MLFQ {mq}",g,metrics(names,arr,bur,f,fr))
run_all("SET A",["P1","P2","P3","P4"],[0,1,2,3],[8,4,9,5],[3,1,4,2],[1,3,5,20],[3,6,12])
run_all("SET B video",["F1","T1","T2","T3","F2","T4"],[0,1,2,3,4,5],[1200,2,2,2,1200,2],[2,1,1,1,2,1],[2,20,100],[2,20,200])
