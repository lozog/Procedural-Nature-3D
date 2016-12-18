#pragma once

// Class for encapsulating index offset and number of indices to be rendered
// for a batch of vertices.  It is assumed that there is a vertex buffer
// setup so that all batch vertices are contiguous in memory and can be rendered
// all at once given a start index offset into the vertex buffer, and a number
// of indices to be rendered.
struct BatchInfo {

	// Starting index within an associated vertex buffer denoting the start
	// of this batch's vertex data.
	unsigned int startIndex;

	// Number of indices to be rendered for this batch.
	unsigned int numIndices;

};

