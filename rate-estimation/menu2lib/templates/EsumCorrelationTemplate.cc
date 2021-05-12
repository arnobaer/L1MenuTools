{% extends "Condition.cc" %}

{% import 'macros.jinja2' as macros %}

{% set objects = cond.getObjects() %}
{% if overlap_removal %}
  {% set reference = objects[objects|length -1] %}
{% endif %}
{% set object = objects[0] if objects[0].getType() in (tmEventSetup.Muon, tmEventSetup.Egamma, tmEventSetup.Tau, tmEventSetup.Jet) else objects[1] %}
{% set esum = objects[1] if objects[1].getType() in (tmEventSetup.ETM, tmEventSetup.HTM, tmEventSetup.ETMHF) else objects[0] %}
{% set prefix = object | getPrefix %}
{% set prefixEsum = esum | getPrefix %}

{% set etaScaleObject = scaleMap | getScale(object, tmGrammar.ETA) %}
{% set nEtaBitsObject = etaScaleObject.getNbits() %}
{% set phiScale = scaleMap | getScale(object, tmGrammar.PHI) %}

{% set LUTS = scaleMap | getLookUpTable(object, esum) %}

{% if esum.getType() == tmEventSetup.ETM -%}
  {% set type = 'L1Analysis::kMissingEt' %}
{% elif object.getType() == tmEventSetup.HTM -%}
  {% set type = 'L1Analysis::kMissingHt' %}
{% elif object.getType() == tmEventSetup.ETMHF -%}
  {% set type = 'L1Analysis::kMissingEtHF' %}
{% endif -%}

{% set iPi = (0.5*(phiScale.getMaximum() - phiScale.getMinimum())/phiScale.getStep()) | int -%}

{% block ConditionLogic %}
  {% if overlap_removal %}
    {{ macros.getReference(reference, tmEventSetup, nEtaBits) }}
  {% endif %}
  size_t nobj = 0;
  for (size_t ii = 0; ii < data->{{prefix}}Bx.size(); ii++)
  {
    if (not (data->{{prefix}}Bx.at(ii) == {{ object.getBxOffset() }})) continue;
    nobj++;
    {% if prefix | isTau %}
      if (nobj > {{macros.getMaxTaus()}}) break;
    {% endif %}
    {{ macros.checkObjectIndex(object, 'nobj') }}
    {% if overlap_removal %}
      {{ cond | hasCorrelationCuts() }}
      {{ macros.removeOverlap(cond, object, 'ii', reference, tmEventSetup, scaleMap, iPi) }}
    {% endif %}
    {{ macros.getObjectCuts(prefix, 'ii', object, tmEventSetup, nEtaBitsObject) }}

    for (size_t jj = 0; jj < data->{{prefixEsum}}Bx.size(); jj++)
    {
      if (not (data->{{prefixEsum}}Type.at(jj) == {{ type }})) continue;
      if (not (data->{{prefixEsum}}Bx.at(jj) == {{ esum.getBxOffset() }})) continue;
      {{ macros.getEsumCuts(prefixEsum, 'jj', esum, tmEventSetup) }}
      {{ macros.getEsumCorrelationCuts(prefix, prefixEsum, 'ii', 'jj', cond, tmEventSetup, LUTS, iPi) }}
      pass = true;
      break;
    }

    if (pass) break;
  }
{% endblock ConditionLogic %}
