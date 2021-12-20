#ifndef SGFLOADER_H
#define SGFLOADER_H

#include "BasicType.h"
#include "SgfTag.h"
#include "WeichiMove.h"
#include "WeichiThreadState.h"

class SgfLoader 
{
	SgfTag m_sgfTag;
	Color m_winColor;
	std::string m_sFileName ;
	std::string m_sSgfString ;
	std::vector<WeichiMove> m_preset ;
	std::vector<WeichiMove> m_play ;
	std::vector<string> m_comment ;

public:
	SgfLoader() { clear(); }
	bool operator()(WeichiThreadState& state) const ;
	bool parseFromFile(const std::string& sgffile, int limit=MAX_GAME_LENGTH) ;
	bool parseFromString(const std::string& sgfstring, int limit=MAX_GAME_LENGTH) ;
	std::vector<WeichiMove> getPlayMove() { return m_play; }
	std::vector<WeichiMove> getPreset() { return m_preset; }
	std::vector<string> getComment() { return m_comment; }
	const std::vector<WeichiMove>& getPlayMove() const { return m_play; }
	const std::vector<WeichiMove>& getPreset() const { return m_preset; }
	const std::vector<string>& getComment() const { return m_comment; }
	Color getWinner() const { return m_winColor; }
	
	// tag info
	std::string getFileName() const { return m_sFileName; }
	std::string getSgfString() const { return m_sSgfString; }
	std::string getEvent() const { return m_sgfTag.getSgfTag("EV"); }
	std::string getDatetime() const { return m_sgfTag.getSgfTag("DT"); }
	std::string getResultInfo() const { return m_sgfTag.getSgfTag("RE"); }
	int getHandicap() const { return atoi(m_sgfTag.getSgfTag("HA").c_str()); }
	uint getBoardSize() const { return atoi(m_sgfTag.getSgfTag("SZ").c_str()); }
	float getKomi() const { return static_cast<float>(atof(m_sgfTag.getSgfTag("KM").c_str())); }
	std::string getPlayerRank(Color color) const { return (color == COLOR_BLACK) ? m_sgfTag.getSgfTag("BR") : m_sgfTag.getSgfTag("WR"); }
	std::string getPlayerName(Color color) const { return (color == COLOR_BLACK) ? m_sgfTag.getSgfTag("PB") : m_sgfTag.getSgfTag("PW"); }
	
	HashKey64 getMinSequenceHashKey() const;

private:
	void clear();
	void addPreset(WeichiMove m) { m_preset.push_back(m) ; }
	void addPlay(WeichiMove m) { m_play.push_back(m); }
	bool handle_property(const std::string& key, const std::string& value);
};

#endif
