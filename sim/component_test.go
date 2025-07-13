package sim

import "testing"

type TestComponent struct{}

func TestCtor(t *testing.T) {
	var _ Component = &TestComponent{}
}
