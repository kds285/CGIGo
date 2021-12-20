#include "SgfLoader.h"
#include "WeichiDynamicKomi.h"

bool SgfLoader::parseFromFile( const std::string& sgffile, int limit/*=MAX_GAME_LENGTH*/ )
{
	this->clear();

	ifstream fin(sgffile.c_str());
	if ( !fin ) return false;

	string sgf ;
	string line ;

	while ( getline(fin, line) ) {
		sgf += line ;
	}
	
	bool bIsSuccess = parseFromString(sgf, limit);
	m_sFileName = sgffile;

	return bIsSuccess;
}

bool SgfLoader::parseFromString( const std::string& sgfstring, int limit/*=MAX_GAME_LENGTH*/ )
{
	this->clear();

	bool slash = false; 
	bool bracket = false ;

	m_sSgfString = sgfstring;

	string key ;
	string value ;
	string buffer ;
	for ( uint p=0; p<sgfstring.length();++p ) {
		if ( slash ) {
			switch (sgfstring[p]) {
			case '\n':  case '\r':  break;
			default: buffer += sgfstring[p] ; break;
			}
			slash = false ;
		} else {
			switch(sgfstring[p]) {
			case '\\':  slash = true; break;
			case ';':
				if ( !bracket ) buffer.clear();
				else buffer += sgfstring[p];
				break;
			case '[':
				if ( !bracket) {
					if ( !buffer.empty() ) key = buffer ;
					bracket = true ;
					buffer.clear();
				}
				break;
			case ']':   
				value = buffer ; buffer.clear(); bracket = false;  
				if (handle_property(key, value) == false) { return false; }
				if ( m_play.size() == limit ) {
					m_play.resize(m_play.size()-1) ;
					return true; /// remove last, and return
				}
				break;
			case ')':   if ( !bracket ) return true; /// ending ~
			default:
				if ( bracket ) buffer += sgfstring[p];
				else if ( isupper(sgfstring[p]) ) buffer += sgfstring[p];
			}
		}
	}
	return false;
}

bool SgfLoader::handle_property( const std::string& key, const std::string& value )
{
	if (key == "AB" || key == "AW") {
		if (!m_play.empty()) return false;
		Color c = toColor(key[1]);
		WeichiMove m(toColor(key[1]), value, getBoardSize());
		if (m.getColor() == COLOR_NONE) { return false; }
		this->addPreset(m);
	} else if (key == "B" || key == "W") {
		Color c = toColor(key[0]);
		WeichiMove m(c, value, getBoardSize());
		if (m.getColor() == COLOR_NONE) { m = WeichiMove(c); }
		this->addPlay(m);
	} else if (key == "C") {
		m_comment.resize(m_play.size());
		m_comment.back() = value;
	} else {
		m_sgfTag.setSgfTag(key, value);
		if (key == "RE") {
			if (value[0] == 'B') { m_winColor = COLOR_BLACK; }
			else if (value[0] == 'W') { m_winColor = COLOR_WHITE; }
			else { m_winColor = COLOR_NONE; }
		}
	}

	return true;
}

bool SgfLoader::operator() ( WeichiThreadState& state ) const
{
	state.resetThreadState();
	WeichiBoard& board = state.m_board ;

	// set AB and AW
	for ( uint i=0;i<m_preset.size();++i ) {
		WeichiMove m = m_preset[i];
		if ( !board.preset(m) ) {
			CERR() << "fail: " << m.toSgfString() << endl;
			return false;
		}
	}

	// set B and W
	for ( uint i=0;i<m_play.size();++i ) {
		WeichiMove m = m_play[i];
		if ( !state.play(m, true) ) {
			CERR() << "fail: " << m.toSgfString() << endl;
			return false;
		}
	}
	return true;
}

HashKey64 SgfLoader::getMinSequenceHashKey() const
{
	HashKey64 minKey = 0;

	for( int symmetric=0; symmetric<SYMMETRY_SIZE; symmetric++ ) {
		HashKey64 key = 0;

		// we take general hash key as preset hash key
		for( uint i=0; i<m_preset.size(); i++ ) {
			WeichiMove rotateMove = m_preset[i].rotateMove(static_cast<SymmetryType>(symmetric));
			key ^= StaticBoard::getHashGenerator().getZHashKeyOf(rotateMove.getColor(),rotateMove.getPosition());
		}

		for( uint i=0; i<m_play.size(); i++ ) {
			WeichiMove rotateMove = m_play[i].rotateMove(static_cast<SymmetryType>(symmetric));
			key ^= StaticBoard::getHashGenerator().getSequenceGameKeys(i,rotateMove.getPosition(),rotateMove.getColor());
		}

		if( symmetric==0 || key<minKey ) { minKey = key; }
	}

	return minKey;
}

void SgfLoader::clear()
{
	m_play.clear();
	m_preset.clear();
	m_sFileName = "";
	m_sSgfString = "";
	m_winColor = COLOR_NONE;
}