#include <iostream>

#include <AnalysisTree/DataHeader.hpp>
#include <AnalysisTree/EventHeader.hpp>
#include <AnalysisTree/Matching.hpp>

#include <AnalysisTree/Chain.hpp>

using namespace AnalysisTree;

int main(int argc, char* argv[]) {

  if (argc < 3) {
    std::cout << "Error! Please use " << std::endl;
    std::cout << " ./example filelist_reco filelist_ana" << std::endl;
    exit(EXIT_FAILURE);
  }

  const std::string filelist_reco = argv[1];
  const std::string filelist_ana = argv[2];

  auto* chain = new Chain(std::vector<std::string>{filelist_reco, filelist_ana}, {"aTree", "cTree"});
  
  auto* config = chain->GetConfiguration();
  auto* data_header = chain->GetDataHeader();

  data_header->Print();
  config->Print();

  EventHeader* sim_event_header{new EventHeader};
  EventHeader* rec_event_header{new EventHeader};
  Container* ana_event_header{new Container};
  Particles* sim_particles{new Particles};
  Particles* rec_particles{new Particles};
  TrackDetector* rec_tracks{new TrackDetector};
  Matching* rec2sim_match{new Matching};

  chain->SetBranchAddress("SimEventHeader", &sim_event_header);
  chain->SetBranchAddress("RecEventHeader", &rec_event_header);
  chain->SetBranchAddress("AnaEventHeader", &ana_event_header);
  chain->SetBranchAddress("VtxTracks", &rec_tracks);
  chain->SetBranchAddress("SimTracks", &sim_particles);
  chain->SetBranchAddress("RecParticlesMcPid", &rec_particles);
  chain->SetBranchAddress("VtxTracks2SimTracks", &rec2sim_match);

  const int dcax_id = config->GetBranchConfig("VtxTracks").GetFieldId("dcax");

  const long n_events = chain->GetEntries();
  std::cout << "Number of entries: " << n_events << std::endl;

  for (long i_event = 0; i_event < 1 /*n_events*/; ++i_event) {
    chain->GetEntry(i_event);

    const int n_tracks = rec_tracks->GetNumberOfChannels();
    const auto centrality = ana_event_header->GetField<float>(0);//NOTE hardcoded 0, to be fixed
    const auto psi_RP = sim_event_header->GetField<float>(0);    //NOTE hardcoded 0, to be fixed
    const TVector3 rec_vtx_x_pos = rec_event_header->GetVertexPosition3();
    const TVector3 sim_vtx_x_pos = sim_event_header->GetVertexPosition3();
    std::cout << " Number of tracks: " << n_tracks << " centrality: " << centrality << " Psi_RP = " << psi_RP << std::endl;

    for (int i_track = 0; i_track < n_tracks; ++i_track) {
      const auto& rec_track = rec_tracks->GetChannel(i_track);
      std::cout << "   RecTrack dcax = " << rec_track.GetField<float>(dcax_id) << std::endl;

      const auto& rec_particle = rec_particles->GetChannel(i_track);
      std::cout << "            phi = " << rec_particle.GetPhi() << std::endl;
      std::cout << "            pid = " << rec_particle.GetPid() << std::endl;

      const int sim_id = rec2sim_match->GetMatch(i_track);
      if (sim_id >= 0) {
        const auto& sim_track = sim_particles->GetChannel(sim_id);
        std::cout << "   SimTrack phi = " << sim_track.GetPhi() << std::endl;
        std::cout << "            pid = " << sim_track.GetPid() << std::endl;
      }
    }
  }
}
