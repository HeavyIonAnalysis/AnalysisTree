// This macro performs run of simple QA tasks of AnalysisTree ROOT files.
// It has two arguments - name of file list and name of tree.
// File list is a text file which contains names of AnalysisTree ROOT files which you want to work with.
// Absolute paths in filelist are strongly recommended to avoid any errors.
// The last symbol in the filelist must be a switch to new line (press ENTER key) -
// otherwise the lst line will not be read properly.

void ReadingMacro(const std::string&& filelist, const std::string&& treename = "rTree") {

  // It was found that on some systems (especially MacOS) initialization of an object from AnalysisTree
  // Infra library (such as Chain) in interactive mode produces a linking error.
  // If it is a case for you, just uncomment the next line:
  // AnalysisTree::Hit();
  // It does nothing, but somehow forward declaration of an object from AnalysisTree Base library
  // (such as Hit) allows to avoid the linking error.

  // AnalysisTree Chain initialization
  AnalysisTree::Chain* treeIn = new AnalysisTree::Chain((std::vector<std::string>){filelist}, (std::vector<std::string>){treename});

  // Initialization of pointers to Containers and Matchings between them
  auto* sim_eve_header = new AnalysisTree::EventHeader();
  auto* sim_tracks = new AnalysisTree::Particles();
  auto* vtx_tracks = new AnalysisTree::TrackDetector();
  auto* tof_hits = new AnalysisTree::HitDetector();
  auto* sim_vtx_matching = new AnalysisTree::Matching();
  auto* tof_vtx_matching = new AnalysisTree::Matching();

  // Initialization of integer field ids with their human-readable string names
  // It is important to do this initialization once, not inside the loop over events
  // or moreover loop over channels, since search by string is quite slow.
  const int chi2_vtx_id = treeIn->GetConfiguration()->GetBranchConfig("VtxTracks").GetFieldId("vtx_chi2");
  const int b_id = treeIn->GetConfiguration()->GetBranchConfig("SimEventHeader").GetFieldId("b");
  const int mass2_id = treeIn->GetConfiguration()->GetBranchConfig("TofHits").GetFieldId("mass2");
  const int qp_id = treeIn->GetConfiguration()->GetBranchConfig("TofHits").GetFieldId("qp_tof");
  const int q_id = treeIn->GetConfiguration()->GetBranchConfig("VtxTracks").GetFieldId("q");

  // Setting addresses of branches.
  // Note that due to specific peculiarities of AnalysisTree branches storing the names of Containers
  // and Matchings specified in SetBranchAddress() function must end with dot symbol '.'.
  treeIn->SetBranchAddress("SimEventHeader.", &sim_eve_header);
  treeIn->SetBranchAddress("SimParticles.", &sim_tracks);
  treeIn->SetBranchAddress("VtxTracks.", &vtx_tracks);
  treeIn->SetBranchAddress("TofHits.", &tof_hits);
  treeIn->SetBranchAddress((treeIn->GetConfiguration()->GetMatchName("VtxTracks", "SimParticles") + ".").c_str(), &sim_vtx_matching);
  treeIn->SetBranchAddress((treeIn->GetConfiguration()->GetMatchName("VtxTracks", "TofHits") + ".").c_str(), &tof_vtx_matching);

  // Setting the number of events to be analized.
  // treeIn->GetEntries() allows to analize all available events, however here you can set
  // the number of events manually, e.g. const int Nevents = 500;
  const int Nevents = treeIn->GetEntries();

  // Create an output file and histograms where the QA output will be stored
  TFile* fileOut = TFile::Open("fileOut.root", "recreate");
  TH1F histo_px("histo_px", "", 1000, -3, 3);
  TH1F histo_b("histo_b", "", 200, 0, 20);
  TH1F histo_mult_lambda("histo_mult_lambda", "", 50, 0, 50);
  TH1F histo_mass2("histo_mass2", "", 100, -1, 2);
  TH1F histo_qp("histo_qp", "", 100, -10, 10);
  TH2F histo_mass2_qp("histo_mass2_qp", "", 100, -10, 10, 100, -1, 2);
  TH2F histo_px_sim_rec("histo_px_sim_rec", "", 500, -3, 3, 500, -3, 3);

  // Set names of axes of output histograms
  histo_px.GetXaxis()->SetTitle("p_{X}, GeV/c");
  histo_b.GetXaxis()->SetTitle("b, fm");
  histo_mult_lambda.GetXaxis()->SetTitle("N_{#Lambda}");
  histo_mass2.GetXaxis()->SetTitle("m^{2}, (GeV/c^{2})^{2}");
  histo_qp.GetXaxis()->SetTitle("p/q, GeV/c");
  histo_mass2_qp.GetXaxis()->SetTitle("p/q, GeV/c");
  histo_mass2_qp.GetYaxis()->SetTitle("m^{2}, (GeV/c^{2})^{2}");
  histo_px_sim_rec.GetXaxis()->SetTitle("p_{X}^{sim}, GeV/c");
  histo_px_sim_rec.GetYaxis()->SetTitle("p_{X}^{rec}, GeV/c");

  for(int i=0; i<Nevents; i++) {  // start a loop over events
    treeIn -> GetEntry(i);        // switch to i-th event

    if(i%100 == 0) {  // print in terminal when switching to each 100-th event
      std::cout << "Event # " << i << "\n";
    }

    // Fill a 1-D histogram with px of vetex tracks - only of those of them
    // which chi2 to the primary vertex does not exceed 3.
    for(const auto& vtx_track : *(vtx_tracks->GetChannels()) ) { // loop over vertex tracks
      const float vtx_chi2 = vtx_track.GetField<float>(chi2_vtx_id);
      if (vtx_chi2 > 3) continue;
      histo_px.Fill(vtx_track.GetPx());
    }

    // Fill a 1-D histogram with impact parameter distribution
    // Note there is no loop over channels, since impact parameter is an event-wise information
    const float b = sim_eve_header->GetField<float>(b_id);
    histo_b.Fill(b);

    // Calculate a multiplicity of lambda baryons in each event
    int N_lambdas = 0;
    for(const auto& sim_track : *(sim_tracks->GetChannels()) ) { // loop over all simulated particles
      if(sim_track.GetPid() == 3122)
        N_lambdas++;
    }
    histo_mult_lambda.Fill(N_lambdas);

    // Fill a 2-D histogram of m2-p from the TOF hits
    for(const auto& tof_hit : *tof_hits) {  // loop over TOF hits
      const float mass2 = tof_hit.GetField<float>(mass2_id);
      const float qp = tof_hit.GetField<float>(qp_id);
      histo_mass2.Fill(mass2);
      histo_qp.Fill(qp);
      histo_mass2_qp.Fill(qp, mass2);
    }

    // Fill a 2-D histogram showing correlation between reconstructed and
    // MC-true (simulated) values of px
    for(const auto& vtx_track : *(vtx_tracks->GetChannels()) ) {  // loop over reconstructed tracks
      const float px_vtx = vtx_track.GetPx();
      // determine unique id of corresponding simulated particle
      const int sim_id = sim_vtx_matching->GetMatch(vtx_track.GetId());
      // id<0 means that there is no matched simulated particle
      // this check is mandatory otherwise the next line will crash
      if(sim_id<0) continue;
      const float px_sim = sim_tracks->GetChannel(sim_id).GetPx();

      histo_px_sim_rec.Fill(px_sim, px_vtx);
    }
  } // end of loop over events

  // Write histograms into the output file
  histo_px.Write();
  histo_b.Write();
  histo_mult_lambda.Write();
  histo_mass2.Write();
  histo_qp.Write();
  histo_mass2_qp.Write();
  histo_px_sim_rec.Write();

  fileOut -> Close();
}
