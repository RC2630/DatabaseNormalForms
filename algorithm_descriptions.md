# Descriptions of Types and Algorithms in this Program

### Types

- Relation (R)
	- contains set\<A> atts
	- contains set\<FD> fds
	- the constructor should perform "find all FD's" (see below) on fds to find implicit FD's and split right sides into size 1

- FunctionalDependency (FD)
	- contains set\<A> left, which determines set\<A> right

- Attribute (A)

### Algorithms

- take closure
	- input: set\<A> atts, set\<FD> fds
	- output: set\<A>
	- steps:
		- add attributes of atts to output
		- loop through fds, for each one, if output contains left side, add right side to output
		- reloop if anything was added during previous loop
		- stop when nothing is added during entire loop

- find all FD's (including implicit FD's)
	- input: set\<FD> fds
	- output: set\<FD>
	- steps:
		- find the closure of the left side of each fd in fds
		- for each closure, translate it to a set of new fds as follows:
		- the left side is the left side of the original fd
		- the right side is all the attributes in the closure not in the left side
		- split this new fd into a set of fds by separating the right side into single attributes
		- remove any trivial fd from fds (trivial means fd.right is contained inside fd.left)
		- remove any irrelevant fd from fds (irrelevant means an attribute appears in the fd but not in the relation)

- lossy or lossless (for decomposition of size 2)
	- input: R rel, set\<R> decomp (of size 2)
	- output: bool
	- steps:
		- take the intersection of the relations in decomp
		- find the closure of the intersection
		- if any of the relations in decomp is a subset of the closure, then it's lossless

- is a superkey
	- input: R rel, set\<A> key
	- output: bool
	- steps:
		- take the closure of key
		- if the closure contains all attributes of rel then it is a superkey

- is a key
	- input: R rel, set\<A> key
	- output: bool
	- steps:
		- find if key is a superkey, if not then it is not a key
		- if it is, then try removing attributes from key one at a time and check if result is a superkey
		- if all attributes attempted to be removed and none are superkeys then key is a minimal key

- find all keys
	- input: R rel
	- output: set\<set\<A>>
	- steps:
		- loop through the rel.fds and place each attribute into one of {left, both, right} based on where it appears in the fds
		- any attribute that does not appear in any fd is put into left
		- take the closure of all atributes in left, and check if that is key
		- if it is, then that is the only key
		- if not, then start adding attributes from both one at a time, and each time check if it is a key
		- each time we find a key, add it to output and do not check any superset of that key (they will not be minimal)
		- continue until every set of attributes that is a superset of left but a subset of {left UNION both} has been checked

- find all superkeys
	- input: R rel
	- output: set\<set\<A>>
	- steps:
		- find all keys
		- for each key, add itself and all supersets of it to output

- does fd violate BCNF
	- input: R rel, FD fd
	- output: bool
	- steps:
		- check if fd.left is a superkey of rel
		- if it is not, then it violates BCNF

- is relation in BCNF
	- input: R rel
	- output: bool
	- steps:
		- for each fd in rel.fds, check to see if it violates BCNF
		- if yes, then the relation is not in BCNF
		- if all fds do not violate BCNF then rel is in BCNF

- BCNF decomp
	- input: R rel
	- output: set\<R>
	- steps:
		- for each fd in rel.fds, check to see if it violates BCNF
		- if it does, then use the fd to split rel into 2 new relations:
		- one with the attributes in fd, and one with the attributes in fd.left + the attributes not in fd
		- then, repeat the process for each of the two new relations, until all relations do not violate BCNF
		- remove redundant relations (relations whose attributes are all found in another relation) from the decomposition

- find minimal cover
	- input: set\<FD> fds
	- output: set\<FD>
	- steps:
		- first, split each fd in fds into separate fds with size of right = 1
		- second, for each fd in the new fds, check to see if any attributes can be removed from fd.left without affecting fd
			- for each fd with left size > 1, try taking away 1 of the attributes from left
			- take the closure of the remaining attributes on the left
			- if the closure contains the removed attribute, then this attribute is not needed, so delete it from the fd
			- continue attempting to remove attributes from fd.left until either:
			- fd.left has size 1 or all remaining attributes cannot be removed from fd
		- third, for each fd in the new fds, check to see if the entire fd can be removed from fds without affecting it
			- for each fd in fds, pretend that fd has been removed
			- take the closure of fd.left (while pretending the above) and check to see if the closure contains all attributes in fd.right
			- if it does, then this fd is redundant, so then actually remove it from fds for good
			- any actually removed fd's must be considered nonexistent for the later FD's considered during the loop
		- the minimal cover is the updated fds after all 3 steps

- does fd violate 3NF
	- input: R rel, FD fd
	- output: bool
	- steps:
		- find all keys of rel
		- check if fd.left is a superkey of rel
		- check if fd.right is part of any key of rel
		- if fd.left is not a superkey AND fd.right is not part of any key, then it violates 3NF

- is relation in 3NF
	- input: R rel
	- output: bool
	- steps:
		- for each fd in rel.fds, check to see if it violates 3NF
		- if yes, then the relation is not in 3NF
		- if all fds do not violate 3NF then rel is in 3NF

- 3NF decomp (lossless join)
	- input: R rel
	- output: set\<R>
	- steps:
		- if rel is already in 3NF then do nothing
		- pick any fd in rel.fds that violates 3NF and split the relation into two using that fd (same type of split as BCNF)
		- then decompose all the way down to BCNF (for both relations obtained from above split)
		- find the minimal cover for rel.fds
		- for each fd in the minimal cover, if there is no relation in the decomposition that contains all of the attributes in fd:
		- add a new relation with the attributes in fd
		- finally, remove redundant relations

- 3NF decomp (synthesis)
	- input: R rel
	- output: set\<R>
	- steps:
		- if rel is already in 3NF then do nothing
		- find the minimal cover of rel.fds
		- for each fd in the minimal cover, add a new relation with the attributes in fd
		- find all valid candidate keys for rel
		- check if there is a relation in the decomposition with all the attributes of a key (i.e. its attributes are a superkey of rel)
		- if there is not, then add a new relation to the decomposition with the attributes of any of the valid keys (pick 1 key only)
		- finally, remove redundant relations

- optimize decomposition: remove redundant relations
	- input: set\<R> decomp
	- output: set\<R>
	- steps:
		- check if any relations in decomp are fully contained within any other relation
		- in other words, check if any relation is a strict subset of another relation
		- if there are, then remove all such subset relations from decomp

- given that a relation is in 3NF, is it possibly not in BCNF?
	- input: R rel (in 3NF)
	- output: bool
	- steps:
		- note that this algorithm only says if rel is DEFINITELY in BCNF or POSSIBLY NOT in BCNF (never DEFINITELY NOT in BCNF)
		- (1) check if rel has multiple valid candidate keys
		- (2) check if rel has at least 1 key with multiple attributes in it
		- (3) check if rel has at least 2 keys with overlapping attributes
		- a simplification is to only to check (3) because in order for (3) to be true, (1) and (2) both need to be true as well
		- if all of (1), (2), and (3) are true, then rel is POSSIBLY NOT in BCNF; otherwise rel is DEFINITELY in BCNF