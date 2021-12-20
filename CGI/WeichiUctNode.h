#ifndef WEICHIUCTNODE_H
#define WEICHIUCTNODE_H

#include "UctNodeImpl.h"
#include "WeichiMove.h"
#include "boost/atomic.hpp"
#include "Dual.h"
#include "Vector.h"

class WeichiUctNode : public UctNodeImpl<WeichiMove, WeichiUctNode> {

	typedef int DTIndex;
private:
	StatisticData m_rave;
	StatisticData m_vnValue;
	StatisticData::data_type m_dProb;
	boost::atomic<bool> m_expandToken;
	boost::atomic<bool> m_updateToken;

	//////////////////////////////////////////////////////////////////////////
	boost::atomic<bool> m_bIsCNNNode;
	boost::atomic<bool> m_bIsInCNNQueue;
	boost::atomic<float> m_fCNNVNValue;
	boost::atomic<float> m_fAdjustChildVNValue;
	boost::atomic<int> m_virtual_loss;
	//////////////////////////////////////////////////////////////////////////

public:
	WeichiUctNode() {
		m_firstChild = UctNodePtr::NULL_PTR;
		m_nChildren = 0;
		m_expandToken = false;
		m_updateToken = false;

		m_bIsCNNNode = false;
		m_bIsInCNNQueue = false;
		m_fCNNVNValue = 0.0f;
		m_fAdjustChildVNValue = 0.0f;
		m_virtual_loss = false;
	}
	WeichiUctNode& operator=(WeichiUctNode& rhs)
	{
		UctNodeImpl<WeichiMove, WeichiUctNode>::operator=(rhs);
		m_rave = rhs.m_rave;
		m_vnValue = rhs.m_vnValue;
		m_dProb = rhs.m_dProb;

		m_expandToken = false;
		m_updateToken = false;

		//////////////////////////////////////////////////////////////////////////
		m_bIsCNNNode = rhs.isCNNNode();
		m_bIsInCNNQueue = rhs.isInCNNQueue();
		m_fCNNVNValue = rhs.getCNNVNValue();
		m_fAdjustChildVNValue = rhs.getAdjustChildVNValue();
		m_virtual_loss = rhs.getVirtualLossCount();
		//////////////////////////////////////////////////////////////////////////

		return *this;
	}

	void reset(WeichiMove move)
	{
		setMove(move);
		m_uctStat.reset();
		clearChildren();
		m_rave.reset();
		m_vnValue.reset();
		m_dProb = 0;
		m_expandToken = false;
		m_updateToken = false;

		//////////////////////////////////////////////////////////////////////////
		m_bIsCNNNode = false;
		m_bIsInCNNQueue = false;
		m_virtual_loss = 0;
		m_fAdjustChildVNValue = 0.0f;
		//////////////////////////////////////////////////////////////////////////
	}

	//////////////////////////////////////////////////////////////////////////
	inline void setCNNNode() { m_bIsCNNNode = true; }
	inline void removeCNNNode() { m_bIsCNNNode = false; }
	inline bool isCNNNode() const { return m_bIsCNNNode; }
	inline void setInCNNQueue() { m_bIsInCNNQueue = true; }
	inline void removeInCNNQueue() { m_bIsInCNNQueue = false; }
	inline bool isInCNNQueue() const { return m_bIsInCNNQueue; }
	inline void setCNNVNValue(float fVNValue) { m_fCNNVNValue = fVNValue; }
	inline void setAdjustChildVNValue(float fAdjustChildVNValue) { m_fAdjustChildVNValue = fAdjustChildVNValue; }
	inline float getCNNVNValue() const { return m_fCNNVNValue; }
	inline float getAdjustChildVNValue() const { return m_fAdjustChildVNValue; }
	inline int getVirtualLossCount() const { return m_virtual_loss; }
	inline void addVirtualLoss(int count) { m_virtual_loss.fetch_add(count); m_uctStat.add(-1, count); }
	inline void removeVirtualLoss(int count) { m_virtual_loss.fetch_sub(count); m_uctStat.remove(-1, count); }
	inline StatisticData getUctDataWithoutVirtualLoss() const {
		StatisticData uctWithoutVirtualLoss = getUctData();
		uctWithoutVirtualLoss.remove(-1, m_virtual_loss);
		return uctWithoutVirtualLoss;
	}
	//////////////////////////////////////////////////////////////////////////
	
	inline Color getToPlay () const { return AgainstColor(m_move.getColor()); }
	inline StatisticData& getRaveData() { return m_rave ; }
	inline const StatisticData& getRaveData() const { return m_rave ; }
	inline StatisticData& getVNValueData() { return m_vnValue ; }
	inline const StatisticData& getVNValueData() const { return m_vnValue ; }
	inline bool hasVNValue() const { return m_vnValue.getCount()>0 ; }
	inline Color getColor () const { return m_move.getColor(); }
	inline uint getPosition () const { return m_move.getPosition(); }
	string toString() const { return m_move.toSgfString(true); }

	inline StatisticData::data_type getProb() const { return m_dProb; }
	inline void setProb( StatisticData::data_type prob ) { m_dProb = prob; }

	void clearChildren()//clear children
	{
		UctNodeImpl<WeichiMove, WeichiUctNode>::clearChildren();
	}
	void getUpdateToken()
	{
		while(m_updateToken.exchange(true)){;}
	}
	void releaseUpdateToken()
	{
		m_updateToken.store(false);
	}
	bool getExpandToken()
	{
		bool from =false;
		return m_expandToken.compare_exchange_strong(from,true);
	}
	void releaseExpandToken()	{ m_expandToken.store(false); }
};

#endif
