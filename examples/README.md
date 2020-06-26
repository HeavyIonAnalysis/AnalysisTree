# Getting started with AnalysisTree

## Input files
Filelists of reco files (copy of CBMROOT reco):

    /lustre/cbm/users/klochkov/cbm/oct19/urqmd_eos0/auau/12agev/mbias/psd44_hole20_pipe0/TGeant3/filelists/rec/

Ana files (with centrality and PID information):
     
     /lustre/cbm/users/klochkov/cbm/oct19/urqmd_eos0/auau/12agev/mbias/psd44_hole20_pipe0/TGeant3/filelists/ana/

## Runnig example

./example filelist_rec filelist_ana

where, for example: 

    filelist_rec=/lustre/cbm/users/klochkov/cbm/oct19/urqmd_eos0/auau/12agev/mbias/psd44_hole20_pipe0/TGeant3/filelists/rec/filelist_000
    filelist_ana=/lustre/cbm/users/klochkov/cbm/oct19/urqmd_eos0/auau/12agev/mbias/psd44_hole20_pipe0/TGeant3/filelists/ana/filelist_000

## Adding new information

All available information is printed at the beginning, for example:

    Branch VtxTracks (id=1) consists of:
      floating fields:
        chi2 (id=0)
        dcax (id=2)
        dcay (id=3)
        dcaz (id=4)
        eta (id=-3)
        p (id=-7)
        pT (id=-2)
        phi (id=-1)
        px (id=-4)
        py (id=-5)
        pz (id=-6)
        vtx_chi2 (id=1)
     integer fields:
       ndf (id=0)
       nhits (id=2)
       nhits_mvd (id=3)
       q (id=1)
     boolean fields: 
     
Each field can be accessed with: 
     
     GetFied<type>(id)
     
In example.cpp this is illustrated with "dcax" field of "VtxTracks" branch:

    const float dcax = track.GetField<float>(dcax_id);

Fields with negative id are also accessible with simple getters, i. e. GetPx(), GetPhi(), ...