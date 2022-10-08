// -----------------------------------------------------------------------------
//    ASPiK Plugin Kernel File:  plugincore.cpp
//
/**
    \file   plugincore.cpp
    \author Will Pirkle
    \date   17-September-2018
    \brief  Implementation file for PluginCore object
    		- http://www.aspikplugins.com
    		- http://www.willpirkle.com
*/
// -----------------------------------------------------------------------------
#include "plugincore.h"
#include "plugindescription.h"
#include "../../Blockdiagram/PluginKernel/guiconstants.h"
#pragma warning (disable : 4244)

/**
\brief PluginCore constructor is launching pad for object initialization

Operations:
- initialize the plugin description (strings, codes, numbers, see initPluginDescriptors())
- setup the plugin's audio I/O channel support
- create the PluginParameter objects that represent the plugin parameters (see FX book if needed)
- create the presets
*/
PluginCore::PluginCore()
{
    // --- describe the plugin; call the helper to init the static parts you setup in plugindescription.h
    initPluginDescriptors();

    // --- default I/O combinations
	// --- for FX plugins
	if (getPluginType() == kFXPlugin)
	{
		addSupportedIOCombination({ kCFMono, kCFMono });
		addSupportedIOCombination({ kCFMono, kCFStereo });
		addSupportedIOCombination({ kCFStereo, kCFStereo });
	}
	else // --- synth plugins have no input, only output
	{
		addSupportedIOCombination({ kCFNone, kCFMono });
		addSupportedIOCombination({ kCFNone, kCFStereo });
	}

	// --- for sidechaining, we support mono and stereo inputs; auxOutputs reserved for future use
	addSupportedAuxIOCombination({ kCFMono, kCFNone });
	addSupportedAuxIOCombination({ kCFStereo, kCFNone });

	// --- create the parameters
    initPluginParameters();

    // --- create the presets
    initPluginPresets();

	delayTapOne[0] = 0;
	delayTapOne[1] = 0;

	delayTapTwo[0] = 0;
	delayTapTwo[1] = 0;

	delayTapThree[0] = 0;
	delayTapThree[1] = 0;

	delayTapFour[0] = 0;
	delayTapFour[1] = 0;

	AudioDelayParameters delayOne = DelayOne.getParameters();
	delayOne.leftDelay_mSec = delayTimeOne;
	delayOne.feedback_Pct = delayFeedbackOne;
	delayOne.delayRatio_Pct = 100;
	delayOne.updateType = delayUpdateType::kLeftPlusRatio;
	delayOne.dryLevel_dB = -60;
	delayOne.wetLevel_dB = 0;
	// --- use helper
	delayOne.algorithm = convertIntToEnum(delayTypeOne, delayAlgorithm);
	// --- set them
	DelayOne.setParameters(delayOne);


	AudioDelayParameters delayTwo = DelayTwo.getParameters();
	delayTwo.leftDelay_mSec = delayTimeTwo;
	delayTwo.feedback_Pct = delayFeedBack2;
	delayTwo.delayRatio_Pct = 100;
	delayTwo.updateType = delayUpdateType::kLeftPlusRatio;
	delayTwo.dryLevel_dB = -60;
	delayTwo.wetLevel_dB = 0;
	// --- use helper
	delayTwo.algorithm = convertIntToEnum(delayTypeTwo, delayAlgorithm);
	// --- set them
	DelayTwo.setParameters(delayTwo);


	AudioFilterParameters delayOneFilterParams = DelayOnefilter.getParameters();
	delayOneFilterParams.fc = delayOneFilterfc;
	delayOneFilterParams.Q = delayOneFilterQ;
	delayOneFilterParams.boostCut_dB = delayOneFilterDb;
	if (delayOneFilterType == 0) {
		delayOneFilterParams.algorithm =  filterAlgorithm::kButterLPF2;
	}
	else if (delayOneFilterType == 1) {
		delayOneFilterParams.algorithm = filterAlgorithm::kButterHPF2;

	}
	else if (delayOneFilterType == 2) {
		delayOneFilterParams.algorithm = filterAlgorithm::kNCQParaEQ;

	}
	else if (delayOneFilterType == 3) {
		delayOneFilterParams.algorithm = filterAlgorithm::kButterBPF2;

	}
	DelayOnefilter.setParameters(delayOneFilterParams);

	AudioFilterParameters delayTwoFilterParams = DelayTwofilter.getParameters();
	delayTwoFilterParams.fc = delayTwoFilterfc;
	delayTwoFilterParams.boostCut_dB = delayTwoFilterdB;
	delayTwoFilterParams.Q = delayTwoFilterQ;
	if (delayTwoFilterType == 0) {
		delayTwoFilterParams.algorithm = filterAlgorithm::kButterLPF2;
	}
	else if (delayTwoFilterType == 1) {
		delayTwoFilterParams.algorithm = filterAlgorithm::kButterHPF2;

	}	
	else if (delayTwoFilterType == 2) {
		delayTwoFilterParams.algorithm = filterAlgorithm::kNCQParaEQ;

	}
	else if (delayTwoFilterType == 3) {
		delayTwoFilterParams.algorithm = filterAlgorithm::kButterBPF2;

	}
	DelayTwofilter.setParameters(delayTwoFilterParams);


	AudioFilterParameters delayThreeParams = DelayThreefilter.getParameters();
	delayThreeParams.fc = 800;
	delayThreeParams.boostCut_dB = 0;
	delayThreeParams.Q = delayModFilterQ;
	delayThreeParams.algorithm = filterAlgorithm::kAPF2;
	DelayThreefilter.setParameters(delayThreeParams);

	/*AudioFilterParameters delayFourParams = DelayFourfilter.getParameters();
	delayFourParams.fc = 800;
	delayFourParams.boostCut_dB = 0;
	delayFourParams.Q = 8;
	delayFourParams.algorithm = filterAlgorithm::kAPF2;
	DelayFourfilter.setParameters(delayFourParams);*/



	AudioDelayParameters delayThree = DelayThree.getParameters();
	delayThree.leftDelay_mSec = delayTimeThree;
	delayThree.feedback_Pct = delayFeedbackThree;
	delayThree.delayRatio_Pct = 100;
	delayThree.updateType = delayUpdateType::kLeftPlusRatio;
	delayThree.dryLevel_dB = -60;
	delayThree.wetLevel_dB = 0;
	// --- use helper
	delayThree.algorithm = convertIntToEnum(1, delayAlgorithm);
	// --- set them
	DelayThree.setParameters(delayThree);

AudioDelayParameters delayFour = DelayFour.getParameters();
delayFour.leftDelay_mSec = delayTimeFour;
delayFour.feedback_Pct = delayFeedbackFour;
delayFour.delayRatio_Pct = 100;
delayFour.updateType = delayUpdateType::kLeftPlusRatio;
delayFour.dryLevel_dB = -60;
delayFour.wetLevel_dB = 0;
	// --- use helper
delayFour.algorithm = convertIntToEnum(1, delayAlgorithm);
	// --- set them
DelayFour.setParameters(delayFour);

OscillatorParameters lfoParams = lfo.getParameters();
lfoParams.frequency_Hz = lfoRate;
if (lfoEffectType == 2)
lfoParams.waveform = generatorWaveform::kSin;
else
lfoParams.waveform = generatorWaveform::kTriangle;
lfo.setParameters(lfoParams);




}

/**
\brief initialize object for a new run of audio; called just before audio streams

Operation:
- store sample rate and bit depth on audioProcDescriptor - this information is globally available to all core functions
- reset your member objects here

\param resetInfo structure of information about current audio format

\return true if operation succeeds, false otherwise
*/
bool PluginCore::reset(ResetInfo& resetInfo)
{
    // --- save for audio processing
    audioProcDescriptor.sampleRate = resetInfo.sampleRate;
    audioProcDescriptor.bitDepth = resetInfo.bitDepth;

	DelayOne.reset(resetInfo.sampleRate);
	// --- create 2 second delay buffers
	DelayOne.createDelayBuffers(resetInfo.sampleRate, 2000.0);

	DelayTwo.reset(resetInfo.sampleRate);
	// --- create 2 second delay buffers
	DelayTwo.createDelayBuffers(resetInfo.sampleRate, 2000.0);

	DelayThree.reset(resetInfo.sampleRate);
	// --- create 2 second delay buffers
	DelayThree.createDelayBuffers(resetInfo.sampleRate, 2000.0);

	DelayFour.reset(resetInfo.sampleRate);
	// --- create 2 second delay buffers
	DelayFour.createDelayBuffers(resetInfo.sampleRate, 2000.0);

	lfo.reset(resetInfo.sampleRate);
	




    // --- other reset inits
    return PluginBase::reset(resetInfo);
}

/**
\brief one-time initialize function called after object creation and before the first reset( ) call

Operation:
- saves structure for the plugin to use; you can also load WAV files or state information here
*/
bool PluginCore::initialize(PluginInfo& pluginInfo)
{
	// --- add one-time init stuff here

	return true;
}

/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- syncInBoundVariables when preProcessAudioBuffers is called, it is *guaranteed* that all GUI control change information
  has been applied to plugin parameters; this binds parameter changes to your underlying variables
- NOTE: postUpdatePluginParameter( ) will be called for all bound variables that are acutally updated; if you need to process
  them individually, do so in that function
- use this function to bulk-transfer the bound variable data into your plugin's member object variables

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
    // --- sync internal variables to GUI parameters; you can also do this manually if you don't
    //     want to use the auto-variable-binding
    syncInBoundVariables();

    return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- FRAME PROCESSING FUNCTION --- //
/*
	This is where you do your DSP processing on a PER-FRAME basis; this means that you are
	processing one sample per channel at a time, and is the default mechanism in ASPiK.

	You will get better performance is you process buffers or blocks instead, HOWEVER for
	some kinds of processing (e.g. ping pong delay) you must process on a per-sample
	basis anyway. This is also easier to understand for most newbies.

	NOTE:
	You can enable and disable frame/buffer procssing in the constructor of this object:

	// --- to process audio frames call:
	processAudioByFrames();

	// --- to process complete DAW buffers call:
	processAudioByBlocks(WANT_WHOLE_BUFFER);

	// --- to process sub-blocks of the incoming DAW buffer in 64 sample blocks call:
	processAudioByBlocks(DEFAULT_AUDIO_BLOCK_SIZE);

	// --- to process sub-blocks of size N, call:
	processAudioByBlocks(N);
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void PluginCore::updateParameters() {




	AudioFilterParameters delayOneFilterParams = DelayOnefilter.getParameters();
	delayOneFilterParams.fc = delayOneFilterfc;
	delayOneFilterParams.Q = delayOneFilterQ;
	delayOneFilterParams.boostCut_dB = delayOneFilterDb;
	if (delayOneFilterType == 0) {
		delayOneFilterParams.algorithm = filterAlgorithm::kButterLPF2;
	}
	else if (delayOneFilterType == 1) {
		delayOneFilterParams.algorithm = filterAlgorithm::kButterHPF2;

	}
	else if (delayOneFilterType == 2) {
		delayOneFilterParams.algorithm = filterAlgorithm::kNCQParaEQ;

	}
	else if (delayOneFilterType == 3) {
		delayOneFilterParams.algorithm = filterAlgorithm::kButterBPF2;

	}
	DelayOnefilter.setParameters(delayOneFilterParams);



	AudioFilterParameters delayFilterTwoParams = DelayTwofilter.getParameters();
	delayFilterTwoParams.fc = delayTwoFilterfc;
	delayFilterTwoParams.boostCut_dB = delayTwoFilterdB;
	delayFilterTwoParams.Q = delayTwoFilterQ;
	if (delayTwoFilterType == 0) {
		delayFilterTwoParams.algorithm = filterAlgorithm::kButterLPF2;
	}
	else if (delayTwoFilterType == 1) {
		delayFilterTwoParams.algorithm = filterAlgorithm::kButterHPF2;

	}
	else if (delayTwoFilterType == 2) {
		delayFilterTwoParams.algorithm = filterAlgorithm::kNCQParaEQ;

	}
	else if (delayTwoFilterType == 3) {
		delayFilterTwoParams.algorithm = filterAlgorithm::kButterBPF2;

	}
	DelayTwofilter.setParameters(delayFilterTwoParams);



	AudioFilterParameters delayThreeFilterParams = DelayThreefilter.getParameters();
	delayThreeFilterParams.Q = delayModFilterQ;
	DelayThreefilter.setParameters(delayThreeFilterParams);




	AudioDelayParameters delayOneParams = DelayOne.getParameters();
	delayOneParams.leftDelay_mSec = delayTimeOne;
	delayOneParams.feedback_Pct = delayFeedbackOne;
 	delayOneParams.updateType = delayUpdateType::kLeftPlusRatio;
	// --- use helper
	delayOneParams.algorithm = convertIntToEnum(delayTypeOne, delayAlgorithm);
	// --- set them
	DelayOne.setParameters(delayOneParams);



	AudioDelayParameters delayTwoParams = DelayTwo.getParameters();
	delayTwoParams.leftDelay_mSec = delayTimeTwo;
	delayTwoParams.feedback_Pct = delayFeedBack2;
	delayTwoParams.updateType = delayUpdateType::kLeftPlusRatio;
	// --- use helper
	delayTwoParams.algorithm = convertIntToEnum(delayTypeTwo, delayAlgorithm);
	// --- set them
	DelayTwo.setParameters(delayTwoParams);



	AudioDelayParameters delayThreeParams = DelayThree.getParameters();
	delayThreeParams.leftDelay_mSec = delayTimeThree;
	delayThreeParams.feedback_Pct = delayFeedbackThree;
	delayThreeParams.updateType = delayUpdateType::kLeftPlusRatio;
	// --- use helper
	// --- set them
	DelayThree.setParameters(delayThreeParams);

	AudioDelayParameters delayFourParams = DelayFour.getParameters();

	delayFourParams.leftDelay_mSec = delayTimeFour;
	delayFourParams.feedback_Pct = delayFeedbackFour;
	delayFourParams.updateType = delayUpdateType::kLeftPlusRatio;

	// --- use helper
	// --- set them
	DelayFour.setParameters(delayFourParams);



	OscillatorParameters lfoParams = lfo.getParameters();
	lfoParams.frequency_Hz = lfoRate;
	if (lfoEffectType == 2)
		lfoParams.waveform = generatorWaveform::kSin;
	else
		lfoParams.waveform = generatorWaveform::kTriangle;
	lfo.setParameters(lfoParams);

}




/**
\brief frame-processing method

Operation:
- decode the plugin type - for synth plugins, fill in the rendering code; for FX plugins, delete the if(synth) portion and add your processing code
- note that MIDI events are fired for each sample interval so that MIDI is tightly sunk with audio
- doSampleAccurateParameterUpdates will perform per-sample interval smoothing

\param processFrameInfo structure of information about *frame* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioFrame(ProcessFrameInfo& processFrameInfo)
{
    // --- fire any MIDI events for this sample interval
    processFrameInfo.midiEventQueue->fireMidiEvents(processFrameInfo.currentFrame);

	// --- do per-frame smoothing
	doParameterSmoothing();

	// --- call your GUI update/cooking function here, now that smoothing has occurred
	//
	//     NOTE:
	//     updateParameters is the name used in Will Pirkle's books for the GUI update function
	//     you may name it what you like - this is where GUI control values are cooked
	//     for the DSP algorithm at hand
	updateParameters();

	double xnL = processFrameInfo.audioInputFrame[0];//< get from your framework: input sample L
	double xnR = processFrameInfo.audioInputFrame[1]; //< get from your framework: input sample R
	float inputsOne[2] = { xnL, xnR };
	float inputsTwo[2] = { xnL, xnR };

	float outputsDelayLineOne[2] = { 0.0, 0.0 };
	float outputsDelayLineTwo[2] = { 0.0, 0.0 };

	// --- process (in, out, 2 inputs, 2 outputs)
	//DelayOne.processAudioFrame(inputsOne, outputsDelayLineOne, 2, 2);
	//DelayTwo.processAudioFrame(inputsTwo, outputsDelayLineTwo, 2, 2);

	



	//LFO
	if (modulationSwitch == 1) {


		SignalGenData lfoOutput = lfo.renderAudioOutput();


		// --- setup delay modulation
		AudioDelayParameters paramDelayOne = DelayThree.getParameters();
		double minDelay_mSec = 0.0;
		double maxDepth_mSec = 0.0;
		// --- set delay times, wet/dry and feedback
		if (lfoEffectType == 1)
		{
			minDelay_mSec = 0.1; maxDepth_mSec = 7.0;

		}
		else if (lfoEffectType == 0)
		{
			minDelay_mSec = 10.0; maxDepth_mSec = 30.0;
			paramDelayOne.feedback_Pct = 0.0;
		}
		else if (lfoEffectType == 2)
		{
			minDelay_mSec = 0.0; maxDepth_mSec = 7.0;
			//paramDelayOne.feedback_Pct = 0.0;
		}
		// --- calc modulated delay times
		double depth = lfoDepth / 100.0;
		double modulationMin = minDelay_mSec;
		double modulationMax = minDelay_mSec + maxDepth_mSec;
		// --- flanger—unipolar
		if (lfoEffectType == 1)
		{
			double modulationValue = bipolarToUnipolar(depth *
				lfoOutput.normalOutput);

			paramDelayOne.leftDelay_mSec =
				doUnipolarModulationFromMin(modulationValue,
					modulationMin, modulationMax);

		}
		else
		{
			double modulationValue = depth * lfoOutput.normalOutput;
			paramDelayOne.leftDelay_mSec =
				doBipolarModulation(modulationValue,

					modulationMin, modulationMax);

		}

		// --- set right delay to match (*Hint Homework!)
	//paramDelayOne.rightDelay_mSec = params.leftDelay_mSec;
	// --- modulate the delay
		DelayThree.setParameters(paramDelayOne);




		// --- setup delay modulation
		AudioDelayParameters paramDelayTwo = DelayFour.getParameters();
		double minDelay_mSec_delayFour = 0.0;
		double maxDepth_mSec_delayFour = 0.0;
		// --- set delay times, wet/dry and feedback
		if (lfoEffectType == 1)
		{
			minDelay_mSec_delayFour = 0.1; maxDepth_mSec_delayFour = 7.0;

		}
		else if (lfoEffectType == 0)
		{
			minDelay_mSec_delayFour = 10.0; maxDepth_mSec_delayFour = 30.0;
			paramDelayTwo.feedback_Pct = 0.0;
		}

		// --- calc modulated delay times
		double depthDelayFour = lfoDepth / 100.0;
		double modulationMinDelayFour = minDelay_mSec_delayFour;
		double modulationMaxDelayFour = minDelay_mSec_delayFour + maxDepth_mSec_delayFour;
		// --- flanger—unipolar
		if (lfoEffectType == 1)
		{
			double modulationValueDelayFour = bipolarToUnipolar(depthDelayFour *
				lfoOutput.normalOutput);

			paramDelayTwo.leftDelay_mSec =
				doUnipolarModulationFromMin(modulationValueDelayFour,
					modulationMinDelayFour, modulationMaxDelayFour);

		}
		else
		{
			double modulationValueDelayFour = depthDelayFour * lfoOutput.normalOutput;
			paramDelayTwo.leftDelay_mSec =
				doBipolarModulation(modulationValueDelayFour, modulationMinDelayFour, modulationMaxDelayFour);

		}

		// --- set right delay to match (*Hint Homework!)
	//paramDelayOne.rightDelay_mSec = params.leftDelay_mSec;
	// --- modulate the delay
		DelayFour.setParameters(paramDelayTwo);




		AudioFilterParameters paramDelayFilterThree = DelayThreefilter.getParameters();


		double paramFilterThreeDepth = lfoDepth / 100.0;

		if (lfoEffectType == 2) {

		double modulationMinFilterThree = 250;
		double modulationMaxFilterThree =3600;

		double modulationValueFilterThree = bipolarToUnipolar(paramFilterThreeDepth *
			lfoOutput.normalOutput);

		paramDelayFilterThree.fc =
			doUnipolarModulationFromMin(modulationValueFilterThree,
				modulationMinFilterThree, modulationMaxFilterThree);



	}

		DelayThreefilter.setParameters(paramDelayFilterThree);

	}















    // --- decode the channelIOConfiguration and process accordingly
    //
	// --- Synth Plugin:
	// --- Synth Plugin --- remove for FX plugins
	if (getPluginType() == kSynthPlugin)
	{
		// --- output silence: change this with your signal render code
		processFrameInfo.audioOutputFrame[0] = 0.0;
		if (processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
			processFrameInfo.audioOutputFrame[1] = 0.0;

		return true;	/// processed
	}

    // --- FX Plugin:
    if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFMono &&
       processFrameInfo.channelIOConfig.outputChannelFormat == kCFMono)
    {
		// --- pass through code: change this with your signal processing
        processFrameInfo.audioOutputFrame[0] = processFrameInfo.audioInputFrame[0];

        return true; /// processed
    }

    // --- Mono-In/Stereo-Out
    else if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFMono &&
       processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
    {
		// --- pass through code: change this with your signal processing
        processFrameInfo.audioOutputFrame[0] = processFrameInfo.audioInputFrame[0];
        processFrameInfo.audioOutputFrame[1] = processFrameInfo.audioInputFrame[0];

        return true; /// processed
    }

    // --- Stereo-In/Stereo-Out
    else if(processFrameInfo.channelIOConfig.inputChannelFormat == kCFStereo &&
       processFrameInfo.channelIOConfig.outputChannelFormat == kCFStereo)
    {

		// --- pass through code: change this with your signal processing
		//processFrameInfo.audioOutputFrame[0] =  0.5 * xnL + 0.5 * (delayGainOne * outputsDelayLineOne[0] + delayGainTwo * outputsDelayLineTwo[0]);
        //processFrameInfo.audioOutputFrame[1] = 0.5 * xnL + 0.5 * (delayGainOne * outputsDelayLineOne[1] + delayGainTwo * outputsDelayLineTwo[1]);

		//float delayLineOneGainL = delayGainOne * outputsDelayLineOne[0];
		//float delayLineOneGainR = delayGainOne * outputsDelayLineOne[1];
		
		//float delayLineTwoGainL = delayGainTwo * outputsDelayLineTwo[0];
		//float delayLineTwoGainR = delayGainTwo * outputsDelayLineTwo[1];


		//float delayLineOneFilteredL = delayTwoOneFeedback* DelayTwofilter.processAudioSample(delayLineTwoGainL) + delayOneSelfFeedback * DelayOnefilter.processAudioSample(delayLineOneGainL) + delayLineOneGainL;

		//float delayLineTwoFilteredL = delayOneTwoFeedback* DelayOnefilter.processAudioSample(delayLineOneGainL) + delayTwoSelfFeedback * DelayTwofilter.processAudioSample(delayLineTwoGainL) + delayLineTwoGainL;

		//float delayLineOneFilteredR = delayTwoOneFeedback* DelayTwofilter.processAudioSample(delayLineTwoGainR) + delayOneSelfFeedback * DelayOnefilter.processAudioSample(delayLineOneGainR) + delayLineOneGainR;

		//float delayLineTwoFilteredR = delayOneTwoFeedback * DelayOnefilter.processAudioSample(delayLineOneGainR) + delayTwoSelfFeedback * DelayTwofilter.processAudioSample(delayLineTwoGainR) + delayLineTwoGainR;
		
		//if (eigthNoteSwitch && quarterNoteSwitch) {
			delayTapOne[0] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample(((delayGainOne * xnL) + (delayTapTwo[0] * (delayTwoOneFeedback * 0.333)) + (delayTapThree[0] * (delayModCrossFeedback *0.33)) + (delayTapFour[0] * (delayModCrossFeedback * 0.33))) / 4));
			delayTapOne[1] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample(((delayGainOne * xnR) + (delayTapTwo[1] * (delayTwoOneFeedback * 0.333)) + (delayTapThree[1] * (delayModCrossFeedback * 0.33)) + (delayTapFour[1] * (delayModCrossFeedback * 0.33))) / 4));

			delayTapTwo[0] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample(((delayGainTwo * xnL) + (delayTapOne[0] * (delayOneTwoFeedback * 0.333)) + (delayTapThree[0] * (delayModCrossFeedback * 0.33)) + (delayTapFour[0] * (delayModCrossFeedback * 0.33))) / 4));
			delayTapTwo[1] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample(((delayGainTwo * xnR) + (delayTapOne[1] * (delayOneTwoFeedback * 0.333)) + (delayTapThree[1] * (delayModCrossFeedback * 0.33)) + (delayTapFour[1] * (delayModCrossFeedback * 0.33))) / 4));

			float test = ((delayGainTwo * xnL) + (delayTapOne[0] * (delayOneTwoFeedback * 0.333)) + (delayTapThree[0] * (delayModCrossFeedback * 0.33)) + (delayTapFour[0] * (delayModCrossFeedback * 0.33)));

 			delayTapThree[0] = DelayThree.processAudioSample(DelayThreefilter.processAudioSample(((delayGainThree * xnL) + (delayTapTwo[0] * (delayTwoOneFeedback * 0.333)) + (delayTapOne[0] * (delayOneTwoFeedback * 0.333)) + (delayTapFour[0] * (delayModCrossFeedback * 0.33))) / 4));
			delayTapThree[1] = DelayThree.processAudioSample(DelayThreefilter.processAudioSample(((delayGainThree * xnR) + (delayTapTwo[1] * (delayTwoOneFeedback * 0.333)) + (delayTapOne[1] * (delayOneTwoFeedback * 0.333)) + (delayTapFour[1] * (delayModCrossFeedback * 0.33))) / 4));

			delayTapFour[0] = DelayFour.processAudioSample(DelayThreefilter.processAudioSample(((delayGainThree * xnL) + (delayTapTwo[0] * (delayTwoOneFeedback * 0.333)) + (delayTapOne[0] * (delayOneTwoFeedback * 0.333)) + (delayTapThree[0] * (delayModCrossFeedback * 0.33))) / 4));
			delayTapFour[1] = DelayFour.processAudioSample(DelayThreefilter.processAudioSample(((delayGainThree * xnR) + (delayTapTwo[1] * (delayTwoOneFeedback * 0.333)) + (delayTapOne[1] * (delayOneTwoFeedback * 0.333)) + (delayTapThree[1] *  (delayModCrossFeedback *0.33))) / 4));







		//}
		//else if(quarterNoteSwitch) {
		//	delayTapOne[0] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnL) + (delayTapOne[0] * delayOneSelfFeedback) + (delayTapTwo[0] * delayTwoOneFeedback) + (delayTapThree[0] * 0.25)) / 4);
			//delayTapOne[1] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnR) + (delayTapOne[1] * delayOneSelfFeedback) + (delayTapTwo[1] * delayTwoOneFeedback) + (delayTapThree[1] * 0.25)) / 4);

//			delayTapTwo[0] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnL) + (delayTapTwo[0] * delayTwoSelfFeedback) + (delayTapOne[0] * delayOneTwoFeedback) + (delayTapThree[0] * 0.25))/ 4);
//			delayTapTwo[1] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnR) + (delayTapTwo[1] * delayTwoSelfFeedback) + (delayTapOne[1] * delayOneTwoFeedback) + (delayTapThree[1] * 0.25))/ 4);


//			delayTapThree[0] = DelayThree.processAudioSample(DelayTwofilter.processAudioSample((delayGainThree * xnL) + (delayTapTwo[0] * 0.5) + (delayTapOne[0] * 0.5) + (delayTapThree[0] * 0.25)) / 4);
//			delayTapThree[1] = DelayThree.processAudioSample(DelayTwofilter.processAudioSample((delayGainThree * xnR) + (delayTapTwo[1] * 0.5) + (delayTapOne[1] * 0.5) + (delayTapThree[1] * 0.25))/ 4);

//			delayTapFour[0] = 0;
//			delayTapFour[1] = 0;
		//}
		//else if (eigthNoteSwitch) {

	/*		delayTapOne[0] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnL) + (delayTapOne[0] * delayOneSelfFeedback) + (delayTapTwo[0] * delayTwoOneFeedback) + (delayTapFour[0] * 0.25)) / 4);
			delayTapOne[1] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnR) + (delayTapOne[1] * delayOneSelfFeedback) + (delayTapTwo[1] * delayTwoOneFeedback) + (delayTapFour[1] * 0.25)) / 4);

			delayTapTwo[0] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnL) + (delayTapTwo[0] * delayTwoSelfFeedback) + (delayTapOne[0] * delayOneTwoFeedback) + (delayTapFour[0] * 0.25)) / 4);
			delayTapTwo[1] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnR) + (delayTapTwo[1] * delayTwoSelfFeedback) + (delayTapOne[1] * delayOneTwoFeedback) + (delayTapFour[1] * 0.25)) / 4);

			delayTapFour[0] = DelayFour.processAudioSample(DelayTwofilter.processAudioSample((delayGainThree * xnL) + (delayTapTwo[0] * 0.5) + (delayTapOne[0] * 0.5)  + (delayTapFour[0] * 0.25)) / 4);
			delayTapFour[1] = DelayFour.processAudioSample(DelayTwofilter.processAudioSample((delayGainThree * xnR) + (delayTapTwo[1] * 0.5) + (delayTapOne[1] * 0.5)  + (delayTapFour[1] * 0.25)) / 4);

			delayTapThree[0] = 0;
			delayTapThree[1] = 0;*/

		//}

		//else {
		/*	delayTapOne[0] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnL) + (delayTapOne[0] * delayOneSelfFeedback) + (delayTapTwo[0] * delayTwoOneFeedback)) / 3);
			delayTapOne[1] = DelayOne.processAudioSample(DelayOnefilter.processAudioSample((delayGainOne * xnR) + (delayTapOne[1] * delayOneSelfFeedback) + (delayTapTwo[1] * delayTwoOneFeedback)) / 3);

			delayTapTwo[0] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnL) + (delayTapTwo[0] * delayTwoSelfFeedback) + (delayTapOne[0] * delayOneTwoFeedback)) / 3);
			delayTapTwo[1] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnR) + (delayTapTwo[1] * delayTwoSelfFeedback) + (delayTapOne[1] * delayOneTwoFeedback)) /  3);

			delayTapThree[0] = 0;
			delayTapThree[1] = 0;

			delayTapFour[0] = 0;
			delayTapFour[1] = 0;*/
		//}
		
		//DelayOne.processAudioFrame(delayTapOne, outputsDelayLineOne, 2, 2);
		//delayTapOne[0] = outputsDelayLineOne[0];
		//delayTapOne[1] = outputsDelayLineOne[1];


		//delayTapTwo[0] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnL) + (delayTapTwo[0] * delayTwoSelfFeedback) + (delayTapOne[0] * delayOneTwoFeedback) + (delayTapThree[0] * 0.25) + (delayTapFour[0] * 0.25))/5);
		//delayTapTwo[1] = DelayTwo.processAudioSample(DelayTwofilter.processAudioSample((delayGainTwo * xnR) + (delayTapTwo[1] * delayTwoSelfFeedback) + (delayTapOne[1] * delayOneTwoFeedback) + (delayTapThree[1] * 0.25) + (delayTapFour[1] * 0.25))/5);
		
		
		
		//DelayTwo.processAudioFrame(delayTapTwo, outputsDelayLineTwo, 2, 2);
		//delayTapTwo[0] = outputsDelayLineTwo[0];
		//delayTapTwo[1] = outputsDelayLineTwo[1];


		
			float tempL = 0;
			float tempR = 0;

		float preSumL = (delayTapOne[0] + delayTapTwo[0] + delayTapThree[0] + delayTapFour[0]) + dry *0.5 * xnL;

		float preSumR = (delayTapOne[1]+ delayTapTwo[1] + delayTapThree[1] + delayTapFour[1]) + dry * 0.5 * xnR;

		//float LRSum = (preSumL + preSumR)/2;
		//float LRDiff = (preSumL - preSumR)/2;

		if (delayOneSolo == 1) {
			tempL += (1-panDelayOne) * delayTapOne[0];
			tempR += (1 + panDelayOne) * delayTapOne[0];

		}
		
		if (delayTwoSolo == 1) {
			tempL += (1 - panDelayTwo) * delayTapTwo[1];
			tempR += (1 + panDelayTwo) * delayTapTwo[1];

		}
		if (delayThreeSolo == 1) {

			tempL += (1 - panDelayMod) * (delayTapThree[0] + delayTapFour[0]);
			tempR += (1 + panDelayTwo) * (delayTapThree[0] + delayTapFour[0]);
		}

		processFrameInfo.audioOutputFrame[0] = tempL + dry * 0.5 * xnL ;
		processFrameInfo.audioOutputFrame[1] = tempR + dry * 0.5 * xnL;;



        return true; /// processed
    }

    return false; /// NOT processed
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- BLOCK/BUFFER PRE-PROCESSING FUNCTION --- //
//      Only used when BLOCKS or BUFFERS are processed
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
\brief pre-process the audio block

Operation:
- fire MIDI events for the audio block; see processMIDIEvent( ) for the code that loads
  the vector on the ProcessBlockInfo structure

\param IMidiEventQueue ASPIK event queue of MIDI events for the entire buffer; this
       function only fires the MIDI events for this audio block

\return true if operation succeeds, false otherwise
*/
bool PluginCore::preProcessAudioBlock(IMidiEventQueue* midiEventQueue)
{
	// --- pre-process the block
	processBlockInfo.clearMidiEvents();

	// --- sample accurate parameter updates
	for (uint32_t sample = processBlockInfo.blockStartIndex;
		sample < processBlockInfo.blockStartIndex + processBlockInfo.blockSize;
		sample++)
	{
		// --- the MIDI handler will load up the vector in processBlockInfo
		if (midiEventQueue)
			midiEventQueue->fireMidiEvents(sample);
	}

	// --- this will do parameter smoothing ONLY ONCE AT THE TOP OF THE BLOCK PROCESSING
	//
	// --- to perform per-sample parameter smoothing, move this line of code, AND your updating
	//     functions (see updateParameters( ) in comment below) into the for( ) loop above
	//     NOTE: smoothing only once per block usually SAVES CPU cycles
	//           smoothing once per sample period usually EATS CPU cycles, potentially unnecessarily
	doParameterSmoothing();

	// --- call your GUI update/cooking function here, now that smoothing has occurred
	//
	//     NOTE:
	//     updateParameters is the name used in Will Pirkle's books for the GUI update function
	//     you may name it what you like - this is where GUI control values are cooked
	//     for the DSP algorithm at hand
	//     NOTE: updating (cooking) only once per block usually SAVES CPU cycles
	//           updating (cooking) once per sample period usually EATS CPU cycles, potentially unnecessarily
	// updateParameters();

	return true;
}


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//	--- BLOCK/BUFFER PROCESSING FUNCTION --- //
/*
This is where you do your DSP processing on a PER-BLOCK basis; this means that you are
processing entire blocks at a time -- each audio channel has its own block to process.

A BUFFER is simply a single block of audio that is the size of the incoming buffer from the
DAW. A BLOCK is a sub-block portion of that buffer.

In the event that the incoming buffer is SMALLER than your requested audio block, the
entire buffer will be sent to this block processing function. This is also true when your
block size is not a divisor of the actual incoming buffer, OR when an incoming buffer
is partially filled (which is rare, but may happen under certain circumstances), resulting
in a "partial block" of data that is smaller than your requested block size.

NOTE:
You can enable and disable frame/buffer procssing in the constructor of this object:

// --- to process audio frames call:
processAudioByFrames();

// --- to process complete DAW buffers call:
processAudioByBlocks(WANT_WHOLE_BUFFER);

// --- to process sub-blocks of the incoming DAW buffer in 64 sample blocks call:
processAudioByBlocks(DEFAULT_AUDIO_BLOCK_SIZE);

// --- to process sub-blocks of size N, call:
processAudioByBlocks(N);
*/
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/**
\brief block or buffer-processing method

Operation:
- process one block of audio data; see example functions for template code
- renderSynthSilence: render a block of 0.0 values (synth, silence when no notes are rendered)
- renderFXPassThrough: pass audio from input to output (FX)

\param processBlockInfo structure of information about *block* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processAudioBlock(ProcessBlockInfo& processBlockInfo)
{
	// --- FX or Synth Render
	//     call your block processing function here
	// --- Synth
	if (getPluginType() == kSynthPlugin)
		renderSynthSilence(processBlockInfo);

	// --- or FX
	else if (getPluginType() == kFXPlugin)
		renderFXPassThrough(processBlockInfo);

	return true;
}


/**
\brief
renders a block of silence (all 0.0 values) as an example
your synth code would render the synth using the MIDI messages and output buffers

Operation:
- process all MIDI events for the block
- perform render into block's audio buffers

\param blockInfo structure of information about *block* processing
\return true if operation succeeds, false otherwise
*/
bool PluginCore::renderSynthSilence(ProcessBlockInfo& blockInfo)
{
	// --- process all MIDI events in this block (same as SynthLab)
	uint32_t midiEvents = blockInfo.getMidiEventCount();
	for (uint32_t i = 0; i < midiEvents; i++)
	{
		// --- get the event
		midiEvent event = *blockInfo.getMidiEvent(i);

		// --- do something with it...
		// myMIDIMessageHandler(event); // <-- you write this
	}

	// --- render a block of audio; here it is silence but in your synth
	//     it will likely be dependent on the MIDI processing you just did above
	for (uint32_t sample = blockInfo.blockStartIndex, i = 0;
		 sample < blockInfo.blockStartIndex + blockInfo.blockSize;
		 sample++, i++)
	{
		// --- write outputs
		for (uint32_t channel = 0; channel < blockInfo.numAudioOutChannels; channel++)
		{
			// --- silence (or, your synthesized block of samples)
			blockInfo.outputs[channel][sample] = 0.0;
		}
	}
	return true;
}

/**
\brief
Renders pass-through code as an example; replace with meaningful DSP for audio goodness

Operation:
- loop over samples in block
- write inputs to outputs, per channel basis

\param blockInfo structure of information about *block* processing
\return true if operation succeeds, false otherwise
*/
bool PluginCore::renderFXPassThrough(ProcessBlockInfo& blockInfo)
{
	// --- block processing -- write to outputs
	for (uint32_t sample = blockInfo.blockStartIndex, i = 0;
		sample < blockInfo.blockStartIndex + blockInfo.blockSize;
		sample++, i++)
	{
		// --- handles multiple channels, but up to you for bookkeeping
		for (uint32_t channel = 0; channel < blockInfo.numAudioOutChannels; channel++)
		{
			// --- pass through code, or your processed FX version
			blockInfo.outputs[channel][sample] = blockInfo.inputs[channel][sample];
		}
	}
	return true;
}


/**
\brief do anything needed prior to arrival of audio buffers

Operation:
- updateOutBoundVariables sends metering data to the GUI meters

\param processInfo structure of information about *buffer* processing

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postProcessAudioBuffers(ProcessBufferInfo& processInfo)
{
	// --- update outbound variables; currently this is meter data only, but could be extended
	//     in the future
	updateOutBoundVariables();

    return true;
}

/**
\brief update the PluginParameter's value based on GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- use base class helper
    setPIParamValue(controlID, controlValue);

    // --- do any post-processing
    postUpdatePluginParameter(controlID, controlValue, paramInfo);

    return true; /// handled
}

/**
\brief update the PluginParameter's value based on *normlaized* GUI control, preset, or data smoothing (thread-safe)

Operation:
- update the parameter's value (with smoothing this initiates another smoothing process)
- call postUpdatePluginParameter to do any further processing

\param controlID the control ID value of the parameter being updated
\param normalizedValue the new control value in normalized form
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::updatePluginParameterNormalized(int32_t controlID, double normalizedValue, ParameterUpdateInfo& paramInfo)
{
	// --- use base class helper, returns actual value
	double controlValue = setPIParamValueNormalized(controlID, normalizedValue, paramInfo.applyTaper);

	// --- do any post-processing
	postUpdatePluginParameter(controlID, controlValue, paramInfo);

	return true; /// handled
}

/**
\brief perform any operations after the plugin parameter has been updated; this is one paradigm for
	   transferring control information into vital plugin variables or member objects. If you use this
	   method you can decode the control ID and then do any cooking that is needed. NOTE: do not
	   overwrite bound variables here - this is ONLY for any extra cooking that is required to convert
	   the GUI data to meaninful coefficients or other specific modifiers.

\param controlID the control ID value of the parameter being updated
\param controlValue the new control value
\param paramInfo structure of information about why this value is being udpated (e.g as a result of a preset being loaded vs. the top of a buffer process cycle)

\return true if operation succeeds, false otherwise
*/
bool PluginCore::postUpdatePluginParameter(int32_t controlID, double controlValue, ParameterUpdateInfo& paramInfo)
{
    // --- now do any post update cooking; be careful with VST Sample Accurate automation
    //     If enabled, then make sure the cooking functions are short and efficient otherwise disable it
    //     for the Parameter involved
    /*switch(controlID)
    {
        case 0:
        {
            return true;    /// handled
        }

        default:
            return false;   /// not handled
    }*/

    return false;
}

/**
\brief has nothing to do with actual variable or updated variable (binding)

CAUTION:
- DO NOT update underlying variables here - this is only for sending GUI updates or letting you
  know that a parameter was changed; it should not change the state of your plugin.

WARNING:
- THIS IS NOT THE PREFERRED WAY TO LINK OR COMBINE CONTROLS TOGETHER. THE PROPER METHOD IS
  TO USE A CUSTOM SUB-CONTROLLER THAT IS PART OF THE GUI OBJECT AND CODE.
  SEE http://www.willpirkle.com for more information

\param controlID the control ID value of the parameter being updated
\param actualValue the new control value

\return true if operation succeeds, false otherwise
*/
bool PluginCore::guiParameterChanged(int32_t controlID, double actualValue)
{
	/*
	switch (controlID)
	{
		case controlID::<your control here>
		{

			return true; // handled
		}

		default:
			break;
	}*/

	return false; /// not handled
}

/**
\brief For Custom View and Custom Sub-Controller Operations

NOTES:
- this is for advanced users only to implement custom view and custom sub-controllers
- see the SDK for examples of use

\param messageInfo a structure containing information about the incoming message

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMessage(MessageInfo& messageInfo)
{
	// --- decode message
	switch (messageInfo.message)
	{
		// --- add customization appearance here
	case PLUGINGUI_DIDOPEN:
	{
		return false;
	}

	// --- NULL pointers so that we don't accidentally use them
	case PLUGINGUI_WILLCLOSE:
	{
		return false;
	}

	// --- update view; this will only be called if the GUI is actually open
	case PLUGINGUI_TIMERPING:
	{
		return false;
	}

	// --- register the custom view, grab the ICustomView interface
	case PLUGINGUI_REGISTER_CUSTOMVIEW:
	{

		return false;
	}

	case PLUGINGUI_REGISTER_SUBCONTROLLER:
	case PLUGINGUI_QUERY_HASUSERCUSTOM:
	case PLUGINGUI_USER_CUSTOMOPEN:
	case PLUGINGUI_USER_CUSTOMCLOSE:
	case PLUGINGUI_EXTERNAL_SET_NORMVALUE:
	case PLUGINGUI_EXTERNAL_SET_ACTUALVALUE:
	{

		return false;
	}

	default:
		break;
	}

	return false; /// not handled
}


/**
\brief process a MIDI event

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param event a structure containing the MIDI event data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::processMIDIEvent(midiEvent& event)
{
	// --- IF PROCESSING AUDIO BLOCKS: push into vector for block processing
	if (!pluginDescriptor.processFrames)
	{
		processBlockInfo.pushMidiEvent(event);
		return true;
	}

	// --- IF PROCESSING AUDIO FRAMES: decode AND service this MIDI event here
	//     for sample accurate MIDI
	// myMIDIMessageHandler(event); // <-- you write this

	return true;
}

/**
\brief (for future use)

NOTES:
- MIDI events are 100% sample accurate; this function will be called repeatedly for every MIDI message
- see the SDK for examples of use

\param vectorJoysickData a structure containing joystick data

\return true if operation succeeds, false otherwise
*/
bool PluginCore::setVectorJoystickParameters(const VectorJoystickData& vectorJoysickData)
{
	return true;
}

/**
\brief create all of your plugin parameters here

\return true if parameters were created, false if they already existed
*/
bool PluginCore::initPluginParameters()
{
	if (pluginParameterMap.size() > 0)
		return false;

	// --- Add your plugin parameter instantiation code bewtween these hex codes
	// **--0xDEA7--**


	// --- Declaration of Plugin Parameter Objects 
	PluginParameter* piParam = nullptr;

	// --- continuous control: Delay Time 1
	piParam = new PluginParameter(controlID::delayTimeOne, "Delay Time 1", "mMsec", controlVariableType::kDouble, 0.000000, 2000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTimeOne, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Time 2
	piParam = new PluginParameter(controlID::delayTimeTwo, "Delay Time 2", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTimeTwo, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Feedback 1
	piParam = new PluginParameter(controlID::delayFeedbackOne, "Delay Feedback 1", "%", controlVariableType::kDouble, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayFeedbackOne, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Feedback 2
	piParam = new PluginParameter(controlID::delayFeedBack2, "Delay Feedback 2", "%", controlVariableType::kFloat, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayFeedBack2, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Delay Gain 1
	piParam = new PluginParameter(controlID::delayGainOne, "Delay Gain 1", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayGainOne, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Gain 2
	piParam = new PluginParameter(controlID::delayGainTwo, "Delay Gain 2", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.500000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayGainTwo, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Dry
	piParam = new PluginParameter(controlID::dry, "Dry", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 1.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&dry, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Delay One Type
	piParam = new PluginParameter(controlID::delayTypeOne, "Delay One Type", "normal,pingpong", "normal");
	piParam->setBoundVariable(&delayTypeOne, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Delay Two Type
	piParam = new PluginParameter(controlID::delayTypeTwo, "Delay Two Type", "normal,pingpong", "normal");
	piParam->setBoundVariable(&delayTypeTwo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Delay One Crossfeedback
	piParam = new PluginParameter(controlID::delayOneTwoFeedback, "Delay One Crossfeedback", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayOneTwoFeedback, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Two Cross feedback
	piParam = new PluginParameter(controlID::delayTwoOneFeedback, "Delay Two Cross feedback", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTwoOneFeedback, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Gain Mod
	piParam = new PluginParameter(controlID::delayGainThree, "Delay Gain Mod", "Units", controlVariableType::kDouble, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayGainThree, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Time 3
	piParam = new PluginParameter(controlID::delayTimeThree, "Delay Time 3", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTimeThree, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Feedback 3
	piParam = new PluginParameter(controlID::delayFeedbackThree, "Delay Feedback 3", "%", controlVariableType::kFloat, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayFeedbackThree, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Delay Time 4
	piParam = new PluginParameter(controlID::delayTimeFour, "Delay Time 4", "mSec", controlVariableType::kDouble, 0.000000, 2000.000000, 250.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTimeFour, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Feedback 4
	piParam = new PluginParameter(controlID::delayFeedbackFour, "Delay Feedback 4", "%", controlVariableType::kFloat, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayFeedbackFour, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Delay Mod Crossfeedback
	piParam = new PluginParameter(controlID::delayModCrossFeedback, "Delay Mod Crossfeedback", "Units", controlVariableType::kFloat, 0.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayModCrossFeedback, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- discrete control: Delay One Filter
	piParam = new PluginParameter(controlID::delayOneFilterType, "Delay One Filter", "lpf,hpf,notch,bpf", "lpf");
	piParam->setBoundVariable(&delayOneFilterType, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Delay One Filter fc
	piParam = new PluginParameter(controlID::delayOneFilterfc, "Delay One Filter fc", "Hz", controlVariableType::kFloat, 200.000000, 20000.000000, 400.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayOneFilterfc, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- discrete control: Delay Two Filter Type
	piParam = new PluginParameter(controlID::delayTwoFilterType, "Delay Two Filter Type", "lpf,hpf,notch,bpf", "lpf");
	piParam->setBoundVariable(&delayTwoFilterType, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Delay Two Filter fc
	piParam = new PluginParameter(controlID::delayTwoFilterfc, "Delay Two Filter fc", "Hz", controlVariableType::kFloat, 20.000000, 20000.000000, 400.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTwoFilterfc, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: LFO Rate
	piParam = new PluginParameter(controlID::lfoRate, "LFO Rate", "Hz", controlVariableType::kDouble, 0.020000, 20.000000, 0.200000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoRate, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: LFO Depth
	piParam = new PluginParameter(controlID::lfoDepth, "LFO Depth", "Units", controlVariableType::kDouble, 0.000000, 100.000000, 50.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&lfoDepth, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- discrete control: Efftct Type
	piParam = new PluginParameter(controlID::lfoEffectType, "Efftct Type", "chorus,flanger,phaser", "chorus");
	piParam->setBoundVariable(&lfoEffectType, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Delay 1 On/Off
	piParam = new PluginParameter(controlID::delayOneSolo, "Delay 1 On/Off", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&delayOneSolo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Delay Two On/Off
	piParam = new PluginParameter(controlID::delayTwoSolo, "Delay Two On/Off", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&delayTwoSolo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Delay 3 On/Off
	piParam = new PluginParameter(controlID::delayThreeSolo, "Delay 3 On/Off", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&delayThreeSolo, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- discrete control: Modulation
	piParam = new PluginParameter(controlID::modulationSwitch, "Modulation", "SWITCH OFF,SWITCH ON", "SWITCH OFF");
	piParam->setBoundVariable(&modulationSwitch, boundVariableType::kInt);
	piParam->setIsDiscreteSwitch(true);
	addPluginParameter(piParam);

	// --- continuous control: Delay Mod Filter Q
	piParam = new PluginParameter(controlID::delayModFilterQ, "Delay Mod Filter Q", "Units", controlVariableType::kDouble, 0.000000, 18.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayModFilterQ, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay One Filter Q
	piParam = new PluginParameter(controlID::delayOneFilterQ, "Delay One Filter Q", "Units", controlVariableType::kDouble, 0.000000, 18.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayOneFilterQ, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay One Filter Boost/Cut
	piParam = new PluginParameter(controlID::delayOneFilterDb, "Delay One Filter Boost/Cut", "dB", controlVariableType::kFloat, -6.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayOneFilterDb, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Delay Two Filter Q
	piParam = new PluginParameter(controlID::delayTwoFilterQ, "Delay Two Filter Q", "Units", controlVariableType::kDouble, 0.000000, 18.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTwoFilterQ, boundVariableType::kDouble);
	addPluginParameter(piParam);

	// --- continuous control: Delay Two Filter Boost/Cut
	piParam = new PluginParameter(controlID::delayTwoFilterdB, "Delay Two Filter Boost/Cut", "dB", controlVariableType::kFloat, -6.000000, 6.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&delayTwoFilterdB, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Pan Delay One
	piParam = new PluginParameter(controlID::panDelayOne, "Pan Delay One", "Units", controlVariableType::kFloat, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&panDelayOne, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Pan Delay Two
	piParam = new PluginParameter(controlID::panDelayTwo, "Pan Delay Two", "Units", controlVariableType::kFloat, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&panDelayTwo, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- continuous control: Pan Delay Mod
	piParam = new PluginParameter(controlID::panDelayMod, "Pan Delay Mod", "Units", controlVariableType::kFloat, -1.000000, 1.000000, 0.000000, taper::kLinearTaper);
	piParam->setParameterSmoothing(true);
	piParam->setSmoothingTimeMsec(100.00);
	piParam->setBoundVariable(&panDelayMod, boundVariableType::kFloat);
	addPluginParameter(piParam);

	// --- Aux Attributes
	AuxParameterAttribute auxAttribute;

	// --- RAFX GUI attributes
	// --- controlID::delayTimeOne
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayTimeOne, auxAttribute);

	// --- controlID::delayTimeTwo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayTimeTwo, auxAttribute);

	// --- controlID::delayFeedbackOne
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayFeedbackOne, auxAttribute);

	// --- controlID::delayFeedBack2
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayFeedBack2, auxAttribute);

	// --- controlID::delayGainOne
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayGainOne, auxAttribute);

	// --- controlID::delayGainTwo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayGainTwo, auxAttribute);

	// --- controlID::dry
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::dry, auxAttribute);

	// --- controlID::delayTypeOne
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delayTypeOne, auxAttribute);

	// --- controlID::delayTypeTwo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delayTypeTwo, auxAttribute);

	// --- controlID::delayOneTwoFeedback
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayOneTwoFeedback, auxAttribute);

	// --- controlID::delayTwoOneFeedback
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayTwoOneFeedback, auxAttribute);

	// --- controlID::delayGainThree
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayGainThree, auxAttribute);

	// --- controlID::delayTimeThree
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayTimeThree, auxAttribute);

	// --- controlID::delayFeedbackThree
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayFeedbackThree, auxAttribute);

	// --- controlID::delayTimeFour
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayTimeFour, auxAttribute);

	// --- controlID::delayFeedbackFour
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayFeedbackFour, auxAttribute);

	// --- controlID::delayModCrossFeedback
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::delayModCrossFeedback, auxAttribute);

	// --- controlID::delayOneFilterType
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delayOneFilterType, auxAttribute);

	// --- controlID::delayOneFilterfc
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayOneFilterfc, auxAttribute);

	// --- controlID::delayTwoFilterType
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::delayTwoFilterType, auxAttribute);

	// --- controlID::delayTwoFilterfc
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayTwoFilterfc, auxAttribute);

	// --- controlID::lfoRate
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoRate, auxAttribute);

	// --- controlID::lfoDepth
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483648);
	setParamAuxAttribute(controlID::lfoDepth, auxAttribute);

	// --- controlID::lfoEffectType
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(805306368);
	setParamAuxAttribute(controlID::lfoEffectType, auxAttribute);

	// --- controlID::delayOneSolo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741826);
	setParamAuxAttribute(controlID::delayOneSolo, auxAttribute);

	// --- controlID::delayTwoSolo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741826);
	setParamAuxAttribute(controlID::delayTwoSolo, auxAttribute);

	// --- controlID::delayThreeSolo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741826);
	setParamAuxAttribute(controlID::delayThreeSolo, auxAttribute);

	// --- controlID::modulationSwitch
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(1073741829);
	setParamAuxAttribute(controlID::modulationSwitch, auxAttribute);

	// --- controlID::delayModFilterQ
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayModFilterQ, auxAttribute);

	// --- controlID::delayOneFilterQ
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayOneFilterQ, auxAttribute);

	// --- controlID::delayOneFilterDb
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayOneFilterDb, auxAttribute);

	// --- controlID::delayTwoFilterQ
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayTwoFilterQ, auxAttribute);

	// --- controlID::delayTwoFilterdB
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483705);
	setParamAuxAttribute(controlID::delayTwoFilterdB, auxAttribute);

	// --- controlID::panDelayOne
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483655);
	setParamAuxAttribute(controlID::panDelayOne, auxAttribute);

	// --- controlID::panDelayTwo
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483655);
	setParamAuxAttribute(controlID::panDelayTwo, auxAttribute);

	// --- controlID::panDelayMod
	auxAttribute.reset(auxGUIIdentifier::guiControlData);
	auxAttribute.setUintAttribute(2147483655);
	setParamAuxAttribute(controlID::panDelayMod, auxAttribute);


	// **--0xEDA5--**

	// --- BONUS Parameter
	// --- SCALE_GUI_SIZE
	PluginParameter* piParamBonus = new PluginParameter(SCALE_GUI_SIZE, "Scale GUI", "tiny,small,medium,normal,large,giant", "normal");
	addPluginParameter(piParamBonus);

	// --- create the super fast access array
	initPluginParameterArray();

	return true;
}

/**
\brief use this method to add new presets to the list

NOTES:
- see the SDK for examples of use
- for non RackAFX users that have large paramter counts, there is a secret GUI control you
  can enable to write C++ code into text files, one per preset. See the SDK or http://www.willpirkle.com for details

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginPresets()
{
	// **--0xFF7A--**

	// --- Plugin Presets 
	int index = 0;
	PresetInfo* preset = nullptr;

	// --- Preset: Factory Preset
	preset = new PresetInfo(index++, "Factory Preset");
	initPresetParameters(preset->presetParameters);
	setPresetParameter(preset->presetParameters, controlID::delayTimeOne, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTimeTwo, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::delayFeedbackOne, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::delayFeedBack2, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::delayGainOne, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::delayGainTwo, 0.500000);
	setPresetParameter(preset->presetParameters, controlID::dry, 1.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTypeOne, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTypeTwo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneTwoFeedback, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoOneFeedback, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayGainThree, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTimeThree, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::delayFeedbackThree, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTimeFour, 250.000000);
	setPresetParameter(preset->presetParameters, controlID::delayFeedbackFour, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::delayModCrossFeedback, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneFilterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneFilterfc, 400.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoFilterType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoFilterfc, 400.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoRate, 0.200000);
	setPresetParameter(preset->presetParameters, controlID::lfoDepth, 50.000000);
	setPresetParameter(preset->presetParameters, controlID::lfoEffectType, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneSolo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoSolo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayThreeSolo, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::modulationSwitch, -0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayModFilterQ, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneFilterQ, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayOneFilterDb, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoFilterQ, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::delayTwoFilterdB, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::panDelayOne, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::panDelayTwo, 0.000000);
	setPresetParameter(preset->presetParameters, controlID::panDelayMod, 0.000000);
	addPreset(preset);


	// **--0xA7FF--**

    return true;
}

/**
\brief setup the plugin description strings, flags and codes; this is ordinarily done through the ASPiKreator or CMake

\return true if operation succeeds, false otherwise
*/
bool PluginCore::initPluginDescriptors()
{
	// --- setup audio procssing style
	//
	// --- kProcessFrames and kBlockSize are set in plugindescription.h
	//
	// --- true:  process audio frames --- less efficient, but easier to understand when starting out
	//     false: process audio blocks --- most efficient, but somewhat more complex code
	pluginDescriptor.processFrames = kProcessFrames;

	// --- for block processing (if pluginDescriptor.processFrame == false),
	//     this is the block size
	processBlockInfo.blockSize = kBlockSize;

    pluginDescriptor.pluginName = PluginCore::getPluginName();
    pluginDescriptor.shortPluginName = PluginCore::getShortPluginName();
    pluginDescriptor.vendorName = PluginCore::getVendorName();
    pluginDescriptor.pluginTypeCode = PluginCore::getPluginType();

	// --- describe the plugin attributes; set according to your needs
	pluginDescriptor.hasSidechain = kWantSidechain;
	pluginDescriptor.latencyInSamples = kLatencyInSamples;
	pluginDescriptor.tailTimeInMSec = kTailTimeMsec;
	pluginDescriptor.infiniteTailVST3 = kVSTInfiniteTail;

    // --- AAX
    apiSpecificInfo.aaxManufacturerID = kManufacturerID;
    apiSpecificInfo.aaxProductID = kAAXProductID;
    apiSpecificInfo.aaxBundleID = kAAXBundleID;  /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.aaxEffectID = "aaxDeveloper.";
    apiSpecificInfo.aaxEffectID.append(PluginCore::getPluginName());
    apiSpecificInfo.aaxPluginCategoryCode = kAAXCategory;

    // --- AU
    apiSpecificInfo.auBundleID = kAUBundleID;   /* MacOS only: this MUST match the bundle identifier in your info.plist file */
    apiSpecificInfo.auBundleName = kAUBundleName;

    // --- VST3
    apiSpecificInfo.vst3FUID = PluginCore::getVSTFUID(); // OLE string format
    apiSpecificInfo.vst3BundleID = kVST3BundleID;/* MacOS only: this MUST match the bundle identifier in your info.plist file */
	apiSpecificInfo.enableVST3SampleAccurateAutomation = kVSTSAA;
	apiSpecificInfo.vst3SampleAccurateGranularity = kVST3SAAGranularity;

    // --- AU and AAX
    apiSpecificInfo.fourCharCode = PluginCore::getFourCharCode();

    return true;
}

// --- static functions required for VST3/AU only --------------------------------------------- //
const char* PluginCore::getPluginBundleName() { return getPluginDescBundleName(); }
const char* PluginCore::getPluginName(){ return kPluginName; }
const char* PluginCore::getShortPluginName(){ return kShortPluginName; }
const char* PluginCore::getVendorName(){ return kVendorName; }
const char* PluginCore::getVendorURL(){ return kVendorURL; }
const char* PluginCore::getVendorEmail(){ return kVendorEmail; }
const char* PluginCore::getAUCocoaViewFactoryName(){ return AU_COCOA_VIEWFACTORY_STRING; }
pluginType PluginCore::getPluginType(){ return kPluginType; }
const char* PluginCore::getVSTFUID(){ return kVSTFUID; }
int32_t PluginCore::getFourCharCode(){ return kFourCharCode; }
