-- the input function 'complex_in' takes a null-terminated string (the
-- textual representation of the type) and turns it into the internal
-- (in memory) representation. You will get a message telling you 'intSet'
-- does not exist yet but that's okay.
 
CREATE FUNCTION intset_in(cstring)
   RETURNS IntSet
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
-- the output function 'complex_out' takes the internal representation and
-- converts it into the textual representation.
 
CREATE FUNCTION intset_out(IntSet)
   RETURNS cstring
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
-- now, we can create the type. The internallength specifies the size of the
-- memory block required to hold the type (we need two 8-byte doubles).
 
CREATE TYPE IntSet (
   input = intset_in,
   output = intset_out,
   alignment = double
);
 
CREATE FUNCTION intset_contain(int, IntSet)
   RETURNS bool
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR ? (
   leftarg = int,
   rightarg = IntSet,
   procedure = intset_contain,
   commutator = ?
);
 
CREATE FUNCTION intset_get_cardinality(IntSet)
   RETURNS int
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR # (
   rightarg = IntSet,
   procedure = intset_get_cardinality
);
 
CREATE FUNCTION intset_is_subset(IntSet, IntSet)
   RETURNS bool
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR >@ (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_is_subset
);
 
CREATE FUNCTION intset_is_reverse_subset(IntSet, IntSet)
   RETURNS bool
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR @< (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_is_reverse_subset
);
 
CREATE FUNCTION intset_equale(IntSet, IntSet)
   RETURNS bool
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR = (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_equale
);
 
CREATE FUNCTION intset_not_equale(IntSet, IntSet)
   RETURNS bool
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR <> (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_not_equale
);
 
CREATE FUNCTION intset_intersection(IntSet, IntSet)
   RETURNS IntSet
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR && (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_intersection
);
 
CREATE FUNCTION intset_union_set(IntSet, IntSet)
   RETURNS IntSet
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR || (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_union_set
);
 
CREATE FUNCTION intset_disjunction(IntSet, IntSet)
   RETURNS IntSet
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR !! (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_disjunction
);
 
CREATE FUNCTION intset_difference(IntSet, IntSet)
   RETURNS IntSet
   AS '/Users/altria/2022Fall/DBcore/postgresql-12.5/src/tutorial/intset'
   LANGUAGE C IMMUTABLE STRICT;
 
CREATE OPERATOR - (
   leftarg = IntSet,
   rightarg = IntSet,
   procedure = intset_difference
);
 