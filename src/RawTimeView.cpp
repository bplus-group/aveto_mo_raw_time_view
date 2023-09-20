/*! \file
*
* \verbatim
******************************************************************************
*                                                                            *
*  Copyright (c) 2020, b-plus GmbH. All rights reserved!                     *
*                                                                            *
*    All rights are exclusively reserved by b-plus GmbH,                     *
*    unless explicitly agreed otherwise.                                     *
*                                                                            *
*    Redistribution in source or any other form,                             *
*    with or without modification, is not permitted.                         *
*                                                                            *
*    You may use this code according to the license terms of b-plus.         *
*    Please contact b-plus at services@b-plus.com to get the actual          *
*    terms and conditions.                                                   *
*                                                                            *
******************************************************************************
\endverbatim
*
* \brief Implementation of Raw Timestamp Visualization Object
* \author Harald Birkholz
* \copyright (C)2015-2022 b-plus technologies GmbH
* \date 25.04.2022
* \version 2.4.3
*
******************************************************************************/
#include "../include/StdAfx.h"
#include "../include/RawTimeView.h"


CRawTimeView::CRawTimeView():
	m_uiMaxEntries(100),
	m_eSyncPointSource(ESyncPointSource::connector)
{
}

bool CRawTimeView::OnCreate()
{
	// Initialize all needed resources.
	auto bSuccess = true;

	SetSize({ 640, 480 });

	bSuccess &= m_Table.Create(this);
	bSuccess &= m_Column0.Create(this, "WCD Data Timestamp", [this](AvVis::CAvPainter& rPainter)
	{
		for (const auto& rit: m_mapTable)
		{
			rPainter << PrettyPrintScientificInteger(std::get<0>(rit.second)) << AVETO::Visual::Support::SNewLine{};
		}
	});
	m_Table.Add(m_Column0);
	bSuccess &= m_Column1.Create(this, "TAI Sync Timestamp", [this](AvVis::CAvPainter& rPainter)
	{
		for (const auto& rit : m_mapTable)
		{
			rPainter << PrettyPrintScientificInteger(std::get<1>(rit.second)) << AVETO::Visual::Support::SNewLine{};
			// Testing completeness with plain print value, negative with missing digits
			//rPainter << std::to_string(std::get<1>(rit.second)) << AVETO::Visual::Support::SNewLine{};
		}
	});
	m_Table.Add(m_Column1);
	bSuccess &= m_Column2.Create(this, "UTC Sync Timestamp", [this](AvVis::CAvPainter& rPainter)
	{
		for (const auto& rit : m_mapTable)
		{
			rPainter << PrettyPrintScientificInteger(std::get<2>(rit.second)) << AVETO::Visual::Support::SNewLine{};
			// Testing bNoSignDot, negative with -.100.000
			//rPainter << PrettyPrintScientificInteger(-100000) << AVETO::Visual::Support::SNewLine{};
		}
	});
	m_Table.Add(m_Column2);
	bSuccess &= m_Column3.Create(this, "Date and Time", [this](AvVis::CAvPainter& rPainter)
	{
		for (const auto& rit : m_mapTable)
		{
			rPainter << std::get<3>(rit.second) << AVETO::Visual::Support::SNewLine{};
			// Testing bNoSignDot, negative with .-10.000
			//rPainter << PrettyPrintScientificInteger(-10000) << AVETO::Visual::Support::SNewLine{};
		}
	});
	m_Table.Add(m_Column3);

	return bSuccess;
}

void CRawTimeView::OnPropChanged(const char* szName, const AvCore::CAvetoVariant& rvarOld,
	const AvCore::CAvetoVariant& rvarNew, AvCore::CPropertyInfo& rPropInfo)
{
	if (std::string("Source of synchronization information") == szName)
	{
		m_mapTable.clear();
		if (rvarNew != ESyncPointSource::connector && m_ptrImitation)
		{
			if (!m_ptrImitation->Reset())
				AVETO_DEBUG("Cannot reset imitation timebase of MO %1", GetName());
		}
	}
}

void CRawTimeView::OnPaint(AvVis::CAvPainter& rPainter)
{
	m_Table.SetColumnVisible(0, true);
	m_Table.SetColumnVisible(1, true);
	m_Table.SetColumnVisible(2, true);
	m_Table.SetColumnVisible(3, true);

	rPainter << m_Table;
}

void CRawTimeView::ProcessData(const AVETO::Core::SDataPacket* rgsPackets, uint32_t uiPackets)
{
	uint32_t uiWCDImitation;
	uint32_t uiTAIImitation;
	uint32_t uiUTCImitation;

	switch(m_eSyncPointSource)
	{
	case ESyncPointSource::connector:
		if (m_ptrImitation == nullptr)
			return;
		
		uiWCDImitation = m_ptrImitation->GetImitation(AVETO::Core::TIMEBASE_ID_WCD);
		uiTAIImitation = m_ptrImitation->GetImitation(AVETO::Core::TIMEBASE_ID_TAI);
		uiUTCImitation = m_ptrImitation->GetImitation(AVETO::Core::TIMEBASE_ID_UTC);
		break;
	case ESyncPointSource::player:
		//uiWCDImitation = AvCore::GetPlayerTimebaseImitation(AVETO::Core::TIMEBASE_ID_WCD);
		//uiTAIImitation = AvCore::GetPlayerTimebaseImitation(AVETO::Core::TIMEBASE_ID_TAI);
		//uiUTCImitation = AvCore::GetPlayerTimebaseImitation(AVETO::Core::TIMEBASE_ID_UTC);
		//break;
	case ESyncPointSource::recorder:
		//uiWCDImitation = AvCore::GetRecorderTimebaseImitation(AVETO::Core::TIMEBASE_ID_WCD);
		//uiTAIImitation = AvCore::GetRecorderTimebaseImitation(AVETO::Core::TIMEBASE_ID_TAI);
		//uiUTCImitation = AvCore::GetRecorderTimebaseImitation(AVETO::Core::TIMEBASE_ID_UTC);
		//break;
	default:
		uiWCDImitation = AVETO::Core::TIMEBASE_ID_WCD;
		uiTAIImitation = AVETO::Core::TIMEBASE_ID_TAI;
		uiUTCImitation = AVETO::Core::TIMEBASE_ID_UTC;
		break;
	}

	// Iterating through the packets.
	for (uint32_t uiIndex = 0; uiIndex < uiPackets; uiIndex++)
	{
		// Wrapping the data packet for more functionality and safety.
		AvCore::SDataPacketPtr ptrPacketSource(rgsPackets[uiIndex]);

		auto uiCnt = ptrPacketSource.GetSeqCnt();
		auto iTm = static_cast<int64_t>(ptrPacketSource.GetTimestamp());
		auto tmTAI = 
			AvCore::MakeTime(
				uiWCDImitation, 
				iTm);
		auto tmUTC = 
			AvCore::MakeTime(
				uiWCDImitation,
				iTm);

		auto bSuccess = true;
		bSuccess &= AvCore::TranslateTime(tmTAI, uiTAIImitation);
		bSuccess &= AvCore::TranslateTime(tmUTC, uiUTCImitation);
		if (bSuccess)
		{
			m_mapTable[uiCnt] = std::make_tuple(iTm, tmTAI.iTime, tmUTC.iTime, PrettyPrintTimestampUTC(tmUTC.iTime));
		}

		while(m_mapTable.size() > m_uiMaxEntries)
		{
			m_mapTable.erase(m_mapTable.begin());
		}
	}
}

void CRawTimeView::ProcessSyncData(const AVETO::Core::SDataPacket* rgsPackets, uint32_t uiPackets)
{
	if (!m_ptrImitation)
	{
		m_ptrImitation = std::make_shared<AvCore::CTimebaseImitation>();
	}

	// Iterating through the packets.
	for (uint32_t uiIndex = 0; uiIndex < uiPackets; uiIndex++)
	{
		// Wrapping the data packet for more functionality and safety.
		AvCore::SDataPacketPtr ptrPacketSource(rgsPackets[uiIndex]);
		const auto pSyncPoints = 
			reinterpret_cast<const AvCore::SSyncPoint*>(
				static_cast<const uint8_t*>(ptrPacketSource->pData) + 
				sizeof(AVETO::Core::SMDSPacket));

		m_ptrImitation->FeedMDSSyncPointToTimebaseImitation(
			pSyncPoints, 
			ptrPacketSource->uiDataLen - sizeof(AVETO::Core::SMDSPacket));
	}
}

std::string CRawTimeView::PrettyPrintScientificInteger(int64_t iNumber)
{
	const auto ssStd = std::to_string(iNumber);
	auto ssRet = std::string();

	const auto uiMsd = ssStd.size();
	for (auto uiInd = 0*uiMsd; uiMsd + 1 - uiInd; ++uiInd)
	{
		const auto bNoSignDot = uiInd > 2 || iNumber >= 0 && uiInd > 0;
		const auto bNoFinalDot = uiInd + 1 < uiMsd;
		const auto bScaleDot = (uiMsd - uiInd) % 3 == 0;
		if (bScaleDot && bNoFinalDot && bNoSignDot)
		{
			ssRet += '.';
		}
		ssRet += ssStd[uiInd];
	}

	return ssRet;
}

std::string CRawTimeView::PrettyPrintTimestampUTC(int64_t iTimestamp)
{
	const auto sInterpret = AvCore::GetTimeInterpret(iTimestamp);

	return std::to_string(sInterpret.uiYear) +
		"-" + std::to_string(sInterpret.uiMonth) +
		"-" + std::to_string(sInterpret.uiDay) +
		"T" + std::to_string(sInterpret.uiHour) +
		":" + std::to_string(sInterpret.uiMinute) +
		":" + std::to_string(sInterpret.dSecond);
}

