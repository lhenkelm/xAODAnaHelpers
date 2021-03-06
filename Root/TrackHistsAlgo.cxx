#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include "AthContainers/ConstDataVector.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODEventInfo/EventInfo.h"
#include "xAODAnaHelpers/HelperFunctions.h"

#include <xAODAnaHelpers/TrackHistsAlgo.h>

// this is needed to distribute the algorithm to the workers
ClassImp(TrackHistsAlgo)

TrackHistsAlgo :: TrackHistsAlgo () :
    Algorithm("TrackHistsAlgo")
{
}

EL::StatusCode TrackHistsAlgo :: setupJob (EL::Job& job)
{
  job.useXAOD();

  // let's initialize the algorithm to use the xAODRootAccess package
  xAOD::Init("TrackHistsAlgo").ignore(); // call before opening first file

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TrackHistsAlgo :: histInitialize ()
{

  ANA_MSG_INFO( m_name );
  ANA_CHECK( xAH::Algorithm::algInitialize());
  // needed here and not in initalize since this is called first
  if( m_inContainerName.empty() || m_detailStr.empty() ){
    ANA_MSG_ERROR( "One or more required configuration values are empty");
    return EL::StatusCode::FAILURE;
  }


  // declare class and add histograms to output
  m_plots = new TrackHists(m_name, m_detailStr);
  ANA_CHECK( m_plots -> initialize());
  m_plots -> record( wk() );

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TrackHistsAlgo :: fileExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode TrackHistsAlgo :: changeInput (bool /*firstFile*/) { return EL::StatusCode::SUCCESS; }

EL::StatusCode TrackHistsAlgo :: initialize ()
{
  ANA_MSG_INFO( "TrackHistsAlgo");
  m_event = wk()->xaodEvent();
  m_store = wk()->xaodStore();
  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TrackHistsAlgo :: execute ()
{
  const xAOD::EventInfo* eventInfo(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(eventInfo, m_eventInfoContainerName, m_event, m_store, msg()) );


  float eventWeight(1);
  if( eventInfo->isAvailable< float >( "mcEventWeight" ) ) {
    eventWeight = eventInfo->auxdecor< float >( "mcEventWeight" );
  }

  const xAOD::TrackParticleContainer* tracks(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(tracks, m_inContainerName, m_event, m_store, msg()) );

  // get primary vertex
  const xAOD::VertexContainer *vertices(nullptr);
  ANA_CHECK( HelperFunctions::retrieve(vertices, m_vertexContainerName, m_event, m_store, msg()) );
  const xAOD::Vertex *pvx = HelperFunctions::getPrimaryVertex(vertices, msg());

  ANA_CHECK( m_plots->execute( tracks, pvx, eventWeight, eventInfo ));

  return EL::StatusCode::SUCCESS;
}

EL::StatusCode TrackHistsAlgo :: postExecute () { return EL::StatusCode::SUCCESS; }
EL::StatusCode TrackHistsAlgo :: finalize () { return EL::StatusCode::SUCCESS; }
EL::StatusCode TrackHistsAlgo :: histFinalize ()
{
  // clean up memory
  if(m_plots) delete m_plots;
  ANA_CHECK( xAH::Algorithm::algFinalize());
  return EL::StatusCode::SUCCESS;
}
