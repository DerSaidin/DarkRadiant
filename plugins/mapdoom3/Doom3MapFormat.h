#ifndef DOOM3MAPFORMAT_H_
#define DOOM3MAPFORMAT_H_

#include "imap.h"
#include "parse.h"

class Doom3MapFormat : 
	public MapFormat,
	public PrimitiveParser
{
public:
	// RegisterableModule implementation
	virtual const std::string& getName() const;
	virtual const StringSet& getDependencies() const;
	virtual void initialiseModule(const ApplicationContext& ctx);
	
	/**
	 * Parse a primitive from the given token stream.
	 */
	scene::INodePtr parsePrimitive(parser::DefTokeniser& tokeniser) const;
  
    /**
     * Read tokens from a map stream and create entities accordingly.
     */
    void readGraph(scene::INodePtr root, TextInputStream& inputStream) const;

	// Write scene graph to an ostream
	void writeGraph(scene::INodePtr root, GraphTraversalFunc traverse, std::ostream& os) const;
};
typedef boost::shared_ptr<Doom3MapFormat> Doom3MapFormatPtr;

#endif /* DOOM3MAPFORMAT_H_ */