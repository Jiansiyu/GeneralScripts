# Optics Cut Scripts

## Need to change according to your file locations


* **Where to save the generated Cut File**

```
TString WorkDir = "/home/newdriver/Learning/GeneralScripts/OptCut/Result/CRex/20210907/RHRS";
```

* **raster off root file location** (optional, but it will save you a lot of time since you donot have to specify the root file location when you run the code)

```

Int_t cutPro(UInt_t runID,UInt_t current_col=3,TString folder="/home/newdriver/Storage/Research/CRex_Experiment/RasterReplay/Replay/Result/DNP_version/") {

```

## Run !!!


```
root 

> cutPro([runID])

```

* click the sieve center
* after click all the sieve center, click 's' in your keyboard, the cut file will be saved in the ```WorkDir```

**It did not behave well?**, 

* change the counter level in linw 
    ```
    1326|  			TList *lcontour1 = (TList*) conts->At(2); // select which contour to be the boundary
    ```

    maybe change to ```3``` or ```4``` or even larger. But remember to change it back for the next column


## @ me
- [Siyu Jian @ uva](mailto:sj9va@virginia.edu)