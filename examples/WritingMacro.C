float PzViaRapidityAndPt(const float y, const float pT);

// This macro illustrates how user can create own AnalysisTree root file.
// Here two branches are created: EventHeader and SimParticles.
// All the fields are filled either with constant values or with random ones.
void WritingMacro(int nEvents = 1000,
                  float averageNLambdasPerEvent = 5,
                  int seed = 0) {

  gRandom->SetSeed(seed);

  // Create the Configuration object
  AnalysisTree::Configuration config_;

  // Create an EventHeader configuration object
  AnalysisTree::BranchConfig SimEventHeaderBranch("SimEventHeader", AnalysisTree::DetType::kEventHeader);

  // Add user-defined fields for EventHeader object - reaction plane angle and impact parameter
  SimEventHeaderBranch.AddField<float>("psi_RP", "");
  SimEventHeaderBranch.AddField<float>("b", "");

  // Add EventHeader's configuration into Configuration object
  config_.AddBranchConfig( SimEventHeaderBranch );

  // Create a pointer to EventHeader object
  AnalysisTree::EventHeader* sim_event_header_ = new AnalysisTree::EventHeader(config_.GetBranchConfig("SimEventHeader").GetId());
  sim_event_header_->Init(SimEventHeaderBranch); // initialize it

  // Determine user-defined fields ids.
  // It is important to do this initialization once, not inside the loop over events
  // or moreover loop over channels, since search by string is quite slow.
  const int psi_RP_field_id_ = config_.GetBranchConfig( sim_event_header_->GetId() ).GetFieldId("psi_RP");
  const int b_field_id_ = config_.GetBranchConfig( sim_event_header_->GetId() ).GetFieldId("b");

  // Do the same for Particles Container for simulated particles
  AnalysisTree::BranchConfig SimTracksBranch("SimParticles", AnalysisTree::DetType::kParticle);
  config_.AddBranchConfig( SimTracksBranch );
  AnalysisTree::Particles* sim_tracks_ = new AnalysisTree::Particles(config_.GetBranchConfig("SimParticles").GetId());
  // Note: no need to call Init() function as for the EventHeader

  // Create and fill the DataHeader object
  AnalysisTree::DataHeader data_header_;
  data_header_.SetSystem("Au+Au");
  data_header_.SetBeamMomentum(12.);

  // Create an output file
  TFile* out_file_ = new TFile("analysisTree.root", "recreate");
  out_file_->cd();

  // Create an output tree and create branches inside it.
  // Note that due to specific peculiarities of AnalysisTree branches storing the names of Containers
  // and Matchings specified in SetBranchAddress() function must end with dot symbol '.'.
  TTree* tree_ = new TTree("aTree", "Analysis Tree, ToyMC simulation");
  tree_ -> Branch("SimEventHeader.", "AnalysisTree::EventHeader", &sim_event_header_);
  tree_ -> Branch("SimParticles.", "AnalysisTree::Particles", &sim_tracks_);

  // Write the configuration of the output AnalysisTree
  config_.Write("Configuration");

  // Define a function for anisotropic flow simulation
  TF1* func = new TF1("f1", "1 + 2*[0]*TMath::Cos(x) + 2*[1]*TMath::Cos(2*x)", 0, 2*TMath::Pi());
  func->SetParameter(0, 0.3); // directed flow
  func->SetParameter(1, -0.2); // elliptic flow

  for(int iEvent = 0; iEvent<nEvents; iEvent++) { // start loop over events

    if(iEvent%1000==0) {  // print in terminal when switching to each 100-th event
      std::cout << iEvent << std::endl;
    }

    // always clear channels of channalized container when starting new event
    sim_tracks_->ClearChannels();

    // Fill user-defined and default fields of the EventHeader
    // Generate random value of the reaction plane angle (azimuthally uniform)
    const float psi_RP = gRandom->Uniform(2*TMath::Pi());
    sim_event_header_ -> SetField(psi_RP, psi_RP_field_id_);
    // Impact parameter is distributed uniformly. It is not a physical case,
    // but this is just for illustration
    sim_event_header_ -> SetField((float)gRandom->Uniform(20), b_field_id_);
    // Vertex position is always at the {0, 0, 0} point. It is not a physical case,
    // but this is just for illustration
    sim_event_header_ -> SetVertexPosition3({0., 0., 0.});

    // Randomize number of lambda-baryons in a specific event
    // according to Poisson distribution
    const int nLambdas = gRandom->Poisson(averageNLambdasPerEvent);

    for(int iLambda = 0; iLambda<nLambdas; iLambda++) { // start loop over lambdas
      // Randomize rapidity according to Gaussian distribution with mean value
      // equal to the midrapidity in lab frame and standard deviation 0.5
      const float y = gRandom->Gaus(1.62179, 0.5);

      // Randomize transverse momentum distribution assuming that its projection
      // into certain axis in transverse plane is distibuted with Gaussian
      // with 0 mean value and standard deviation 2 GeV/c
      const float pT = std::sqrt(std::pow(gRandom->Gaus(0, 2), 2) +
                                 std::pow(gRandom->Gaus(0, 2), 2));

      // Randomize azimuthal angle distribution assuming that its deviation from the
      // reaction plane angle is distributed with pre-defined directed and elliptic flow values.
      const float phi = psi_RP + func->GetRandom();

      // Calculate lambda's momentum in Cartesian basis
      const float px_sim = pT*TMath::Cos(phi);
      const float py_sim = pT*TMath::Sin(phi);
      const float pz_sim = PzViaRapidityAndPt(y, pT);

      // Create and initialize a new channel for certain lambda-baryon
      auto* simtrack = sim_tracks_->AddChannel();
      simtrack->Init(config_.GetBranchConfig(sim_tracks_->GetId()));

      // Set default fields of it
      simtrack->SetIsAllowedSetMassAndChargeExplicitly();
      simtrack->SetMomentum(px_sim, py_sim, pz_sim);
      simtrack->SetPid(3122);
      simtrack->SetMass(1.115683);
    } // end of loop over lambda-baryons in a single event

    // Fill the event into the output tree
    tree_->Fill();
  } // end of loop over events

  // Write DataHeader and Tree into the file
  data_header_.Write("DataHeader");
  tree_->Write();

  // Close file
  out_file_->Close();
}

float PzViaRapidityAndPt(const float y, const float pT) {
  const float m = 1.115683;
  return (TMath::Sqrt(m*m+pT*pT)*(TMath::Exp(2*y) - 1.)) / (2*TMath::Exp(y));
}
